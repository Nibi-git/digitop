//программа для устройства защиты бытовой техники от превышения напряжения в сети
//на основе Tiny26

#include <ioavr.h>
#include <intrinsics.h>
#include "hardware.h"
#include "segments.h"

__eeprom unsigned int UpTresholdEEPROM, DownTresholdEEPROM, K1EEPROM, ProtectTimerOnMaxValueEEPROM;

unsigned char segmentsDec[3]={100,10,1}; 
volatile unsigned char ResultSummReady;
volatile unsigned char SlowTimerFlag;
unsigned char SlowTimerPrescaler;
unsigned char String[3];
unsigned char cycle_count;
volatile unsigned char VideoBuffer[3];
unsigned char keypress[2];
unsigned int KeyTimer;
volatile unsigned int ProtectTimerOnValue;
unsigned int ProtectTimerOnMaxValue;

volatile unsigned char ProtectTimerOffValue;

unsigned int Voltage;
volatile unsigned int UpTreshold, DownTreshold; //volatile 
unsigned int K1; //volatile 
unsigned int DisplayedVoltage;
volatile unsigned char DisplayMode = RealtimeVoltage, EnableDisplay = HALF_BYTE_CONST; // небольшая экономия кода

#ifdef METHOD_PEAK 
volatile unsigned int PeakResult;
unsigned int Peak; // внутреннее значение
#endif

#ifdef METHOD_DIRECT_SHOW 
volatile unsigned int PeakResult;
#endif

volatile unsigned char PlusSamplesCounter, ZeroSamplesCounter;
volatile unsigned long SummUUResult;
unsigned long SummUU; // внутреннее значение

#ifdef SMOOTH_MEASURE
unsigned int SmoothVoltage[SmoothBufferSize]; // буфер для сглаживания индикации
unsigned char SmoothVoltageCurrentPosition; // указатель - куда сейчас пишем
#endif

unsigned char QuarterSecPrescaler, SecPrescaler;

#ifdef METHOD_DIRECT_SHOW
volatile unsigned char MaxSampleNum;
#endif

#ifdef RELAY_TEST  
unsigned char RelayTestCounter;  
#endif
  
void __watchdog_init (void)
{
//запускаю сторожевой таймер на 2 секунды
__watchdog_reset ();
WDTCR |= ((1<<WDCE)|(1<<WDE));
WDTCR = (1<<WDE)|(7<<WDP0);
__watchdog_reset ();
}

void CharToStringDec(signed int inp) // обрезанная до сотен
{
unsigned char i;
if (inp > 999) inp = 999;
String[0]=String[1]=String[2]=0;
// перевод
for(i=0;i<3;)
  {
  if((inp-segmentsDec[i])>=0)
    {
    inp-=segmentsDec[i];
    String[i]++;
    }
    else i++;
  }
}

void InitPorts (void)
{
PortButton |= (1<<Button_DN)|(1<<Button_UP); // подтягивающие резисторы

PortAnodeDir |= ((1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(1<<SEG_DP));
PortKathodeAndRelayDir |= ((1<<Kathode_1) | (1<<Kathode_2) | (1<<Kathode_3)|(1<<Relay));
}

void InitADC (void)
{
ADMUX = ((2<<REFS0)|(InADC<< MUX0));
ADCSR = ((1 << ADEN)|(5 << ADPS0)|(0<<ADFR)|(0<<ADSC)|(0<<ADIE)); 
}

void InitTimers (void)
{
TCCR1B |= (4<<CS10);//4-ck/8
TIMSK |= (1<<TOIE1);
}

void SaveSettings (void)
{
UpTresholdEEPROM = UpTreshold;
DownTresholdEEPROM = DownTreshold;
ProtectTimerOnMaxValueEEPROM = ProtectTimerOnMaxValue;
K1EEPROM = K1;
}

void CheckAllSettings (void)
{
if (UpTreshold > UP_TRESHOLD_MAX) UpTreshold = UP_TRESHOLD_MAX;
if (UpTreshold < UP_TRESHOLD_MIN) UpTreshold = UP_TRESHOLD_MIN;

if (DownTreshold > DOWN_TRESHOLD_MAX) DownTreshold = DOWN_TRESHOLD_MAX;
if (DownTreshold < DOWN_TRESHOLD_MIN) DownTreshold = DOWN_TRESHOLD_MIN;

if (ProtectTimerOnMaxValue > PROTECT_TIMER_ON_MAX) ProtectTimerOnMaxValue = PROTECT_TIMER_ON_MAX;
if (ProtectTimerOnMaxValue < PROTECT_TIMER_ON_MIN) ProtectTimerOnMaxValue = PROTECT_TIMER_ON_MIN;

if (K1 > K1_MAX) K1 = K1_MAX;
if (K1 < K1_MIN) K1 = K1_MIN;
}

void LoadSettings (void)
{
UpTreshold = UpTresholdEEPROM;
DownTreshold = DownTresholdEEPROM;
ProtectTimerOnMaxValue = ProtectTimerOnMaxValueEEPROM;
K1 = K1EEPROM; 
CheckAllSettings ();
}

void UpdateLedScreen (void)
{
switch (DisplayMode)
  {
  case RealtimeVoltage:     CharToStringDec(DisplayedVoltage);      break;
  case UpTresholdVoltage:   CharToStringDec(UpTreshold);            break;
  case DownTresholdVoltage: CharToStringDec(DownTreshold);          break;
  case UpTresholdTune:      CharToStringDec(UpTreshold);            break;
  case DownTresholdTune:    CharToStringDec(DownTreshold);          break;
  case ProtectTimerTune:    CharToStringDec(ProtectTimerOnMaxValue);  break;
  case K1Tune:              CharToStringDec(K1);                    break;
  case ProtectTimer:        CharToStringDec(ProtectTimerOnValue);     break;
  }

if (EnableDisplay)
  {
  for (unsigned char i=0; i<3; i++) {VideoBuffer[i] = led_digits[String[i]];}
  if (DisplayMode >= UpTresholdTune)    {    VideoBuffer[2] |= led_digits[0x10];    }
  }
  else
    {
    VideoBuffer[0] = 0;
    VideoBuffer[1] = 0;
    VideoBuffer[2] = 0;
    }
}

inline void ResetSampler (void) // экономим время на вызов функции
{
SummUU = 0;
PlusSamplesCounter = 0;
ZeroSamplesCounter = 0;
ResultSummReady = 1;
}

#pragma vector = TIMER1_OVF1_vect 
__interrupt void ADCSampleReady (void)
{
TCNT1 = T1_RELOAD; // перезагрузить таймер
unsigned long temp = ADC;
ADCSR |= (1<<ADSC);

if (temp > DACNoiseTreshold) 
  {
#ifdef METHOD_AR
  if (PlusSamplesCounter < RMSMeasureLength) SummUU += temp;
#endif
#ifdef METHOD_RMS
  if (PlusSamplesCounter < RMSMeasureLength) SummUU += temp*temp;
#endif  
#ifdef METHOD_DIRECT_SHOW
  MaxSampleNum ++; // считаем количество положительных семплов
#endif
#ifdef METHOD_PEAK  
  if (PlusSamplesCounter == 64) Peak = temp; // узнаем значение по конкретному семплу
#endif
  
  PlusSamplesCounter++;
  ZeroSamplesCounter = 0;

  if (PlusSamplesCounter > (HalfWaveMeasureLength*1.9))// рапортуем об обрыве или повреждении
    {
    SummUUResult = 0;
    SummUU = 0;

#ifdef METHOD_PEAK
    Peak = 0;
    PeakResult = 0;           
#endif          
   
#ifdef METHOD_DIRECT_SHOW          
    MaxSampleNum = 0;
#endif          
    ResetSampler ();
    }
  }

  else 
    { // считаем сколько нулевых семплов попалось
    ZeroSamplesCounter++;
    
    if ((ZeroSamplesCounter > (HalfWaveMeasureLength/5))&&(PlusSamplesCounter > (HalfWaveMeasureLength/20))) // полуволна закончилась
      {
#ifdef METHOD_DIRECT_SHOW       
      PeakResult = MaxSampleNum; 
      MaxSampleNum = 0;
#endif      
#ifdef METHOD_PEAK
      PeakResult = Peak;           
      Peak = 0;
#endif
      SummUUResult = SummUU;
      SummUU = 0;
      ResetSampler ();
      }
  
    if (ZeroSamplesCounter > HalfWaveMeasureLength*1.9) // рапортуем, что напряжение пропало совсем
      {
      SummUU = 0;
      SummUUResult = 0;
#ifdef METHOD_PEAK
      Peak = 0;
      PeakResult = 0;           
#endif
#ifdef METHOD_DIRECT_SHOW       
      MaxSampleNum = 0;
#endif      
      ResetSampler ();
      }
    }

if (++SlowTimerPrescaler >= RMSMeasureLength)
  {
  SlowTimerPrescaler = 0;
  SlowTimerFlag = 1;
  }
}

void PressProcessing (unsigned char code_state)
{
switch (code_state)
  {
  case DN_SHORT:
    switch (DisplayMode)
    {
    case RealtimeVoltage:       DisplayMode = DownTresholdVoltage;                break;
    case UpTresholdVoltage:     DisplayMode = DownTresholdVoltage;                break;
    case DownTresholdTune:      DownTreshold --;                                  break;
    case UpTresholdTune:        UpTreshold --;                                    break;
    case ProtectTimerTune:      ProtectTimerOnMaxValue -=PROTECT_TIMER_STEP;      break;
    case ProtectTimer:          DisplayMode = DownTresholdVoltage;                break;
    case K1Tune:                K1 --;                                            break;
    }
  break;
  
  case UP_SHORT:
    switch (DisplayMode)
    {
    case RealtimeVoltage:       DisplayMode = UpTresholdVoltage;                  break;
    case DownTresholdVoltage:   DisplayMode = UpTresholdVoltage;                  break;
    case DownTresholdTune:      DownTreshold ++;                                  break;
    case UpTresholdTune:        UpTreshold ++;                                    break;
    case ProtectTimerTune:      ProtectTimerOnMaxValue +=PROTECT_TIMER_STEP;      break;
    case ProtectTimer:          DisplayMode = UpTresholdVoltage;                  break;  
    case K1Tune:                K1 ++;                                            break;  
    }
  break;
  
  case DN_MID:
  if (DisplayMode == DownTresholdVoltage) DisplayMode = DownTresholdTune;
  break;
  
  case UP_MID:
  if (DisplayMode == UpTresholdVoltage) DisplayMode = UpTresholdTune;
  break;
  
  case DN_UP_MID:
    switch (DisplayMode)
    {
    case RealtimeVoltage:      DisplayMode = ProtectTimerTune;                    break;
    case ProtectTimerTune:     DisplayMode = K1Tune;                              break;
    }
  break;
  }
CheckAllSettings ();
}

//#pragma vector = TIMER0_OVF0_vect 
//__interrupt void DynInd (void)

void DynIndStep (void)
{
//TCNT0 = T0_RELOAD; // перезагрузить таймер
if (cycle_count < NUMBER_DIGITS)
  {
  PortKathode |= ((1<<Kathode_1) | (1<<Kathode_2) | (1<<Kathode_3)); //потушили все разряды
  PortAnode &= ~((1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(1<<SEG_DP));

  PortAnode = VideoBuffer[cycle_count]; // выдать новые значения для анодов
  PortKathode &= ~ArrayKathodes[cycle_count];// включить катод
  }

if (++cycle_count >= NUMBER_DIGITS) cycle_count=0;
}

void CheckButton (unsigned char button, unsigned char counter, unsigned char event)
{
  if (!(PinButton & (1<<button))) {counter++; KeyTimer = KeyTimerMax;}
  else 
    {
    if ((counter <  midpress)&&(counter >  shortpress)) PressProcessing(event);
    counter=0;
    }
}

void KeyboardThread (void)
{
if (cycle_count == 2) // проверяем левую кнопку
  {
  if (!(PinButton & (1<<Button_DN))) {keypress[0]++; KeyTimer = KeyTimerMax;}
  else 
    {
    if ((keypress[0] <  midpress)&&(keypress[0] >  shortpress)) PressProcessing(DN_SHORT);
    keypress[0]=0;
    }
  }

if (cycle_count == 0) // проверяем правую кнопку
  {
  if (!(PinButton & (1<<Button_UP))) {keypress[1]++; KeyTimer = KeyTimerMax;}
  else 
    {
    if ((keypress[1] <  midpress)&&(keypress[1] >  shortpress)) PressProcessing(UP_SHORT);
    keypress[1]=0;
    }
  }

if (keypress[1] == 0) // была нажата кнопка ВНИЗ
{
  if (keypress[0] == midpress) {  keypress[0] = midpress+5;  PressProcessing(DN_MID);  }
  if (keypress[0] > longpress) {  keypress[0] = longpress+5; } 

  if ((keypress[0] == 0)&&(KeyTimer)) KeyTimer--; // или не нажаты обе кнопки
}

if (keypress[0] == 0) // была нажата кнопка ВВЕРХ
{
  if (keypress[1] == midpress) {  keypress[1] = midpress+5;  PressProcessing(UP_MID);  }  
  if (keypress[1] > longpress) {  keypress[1] = longpress+5; }
}

if ((keypress[0] == midpress)&&(keypress[1] > shortpress))  {  keypress[0] = midpress+5; keypress[1] = midpress+5; PressProcessing(DN_UP_MID);  }
if ((keypress[0] > longpress)&&(keypress[1] > longpress))  {  keypress[0] = longpress+5; keypress[1] = longpress+5; }

if (KeyTimer == 1)   
  {
  if (DisplayMode >= UpTresholdTune) SaveSettings ();
  DisplayMode = RealtimeVoltage;  
  }
}

#ifdef METHOD_RMS  
unsigned long isqrt (unsigned long v)
{
unsigned long temp, nHat=0, b=0x8000, bshft=15;
do{
  if (v >= (temp = (((nHat<<1)+b)<<bshft--)))    {    nHat += b;    v -= temp;    }
  }while (b>>=1);
return nHat;
}
#endif

int main( void )
{
InitPorts ();
InitTimers ();
InitADC ();
LoadSettings ();

#ifdef OVERRIDE_SETTINGS
  UpTreshold = OVERRIDE_UP_TRESHOLD_VALUE;
  DownTreshold = OVERRIDE_DOWN_TRESHOLD_VALUE;
  ProtectTimerOnMaxValue = OVERRIDE_PROTECT_TIMER_MAX_VALUE;
  K1 = OVERRIDE_K1_VALUE;
  SaveSettings ();
#endif

ProtectTimerOnValue = ProtectTimerOnMaxValue;	//таймер включения после провалов взведен
__watchdog_init ();
__enable_interrupt();

  while (1)
  {
  //__delay_cycles((CtrlClockRate/1000)*5);
  if (SlowTimerFlag)
  {
  SlowTimerFlag = 0;
  if (ResultSummReady)
    {
    __watchdog_reset ();
    ResultSummReady = 0;
#ifdef METHOD_PEAK    
    Voltage = (unsigned int)(((unsigned long)PeakResult*500)>>10);
#endif    
#ifdef METHOD_DIRECT_SHOW    
    Voltage = PeakResult;
#endif    
#ifdef METHOD_AR
    Voltage = (unsigned int)(((SummUUResult>>6)*K1)>>10);
#endif
#ifdef METHOD_RMS  
    Voltage = (unsigned int)((isqrt(SummUUResult>>6)*K1)>>10);
#endif    
    }
    
    //быстрая защита
    if ( (Voltage > UpTreshold) || ((Voltage < DownTreshold)&& (Voltage < DOWN_TRESHOLD_FAST_MIN)) ) ProtectTimerOnValue = ProtectTimerOnMaxValue; //
    
    //медленная защита  
    if  ((Voltage < DownTreshold) && (Voltage >= DOWN_TRESHOLD_FAST_MIN)) 
      {
      if (ProtectTimerOnValue) ProtectTimerOffValue=0; // если уже и так выключено - сразу сгоняем таймер выключения в 0
      if (ProtectTimerOffValue) ProtectTimerOffValue--; // тикает таймер выключения
      }
      else ProtectTimerOffValue = ProtectTimerOffMaxValue;
    if (ProtectTimerOffValue == 0) ProtectTimerOnValue = ProtectTimerOnMaxValue;
    
#ifndef RELAY_TEST    
    if (ProtectTimerOnValue) PortRelay &= ~(1<<Relay);
      else   PortRelay |= (1<<Relay);     
#endif         
      
#ifndef KEYBOARD_DISABLE      
    KeyboardThread ();
#endif 
   
    if ((DisplayMode == ProtectTimer)&&((ProtectTimerOnValue == ProtectTimerOnMaxValue) || (ProtectTimerOnValue == 0))) DisplayMode = RealtimeVoltage;
    if ((DisplayMode == RealtimeVoltage)&&(ProtectTimerOnValue == (ProtectTimerOnMaxValue - 1))) DisplayMode = ProtectTimer;

    if (++QuarterSecPrescaler >= (ProtectTimer1S/8))  //раз в 1/8 секунды сюда заходим
      {
      QuarterSecPrescaler = 0;
  
#ifdef SMOOTH_MEASURE
      SmoothVoltage [SmoothVoltageCurrentPosition] = Voltage; 
      if (++SmoothVoltageCurrentPosition >= SmoothBufferSize) SmoothVoltageCurrentPosition = 0;
      DisplayedVoltage = 0;
      for (unsigned char v=0; v<SmoothBufferSize; v++) DisplayedVoltage += SmoothVoltage [v];
      DisplayedVoltage >>= SmoothShift;
#else
      DisplayedVoltage = Voltage;
#endif
      
      if ((ProtectTimerOnValue)&&(DisplayMode == RealtimeVoltage)) EnableDisplay += HALF_BYTE_CONST; // мигаем экраном
        else EnableDisplay = HALF_BYTE_CONST; // не мигаем
      }
    
    UpdateLedScreen ();    
    DynIndStep ();

#ifdef RELAY_TEST     
    RelayTestCounter++;
    if (RelayTestCounter == 0) PortRelay &= ~(1<<Relay);
    if (RelayTestCounter == HALF_BYTE_CONST) PortRelay |= (1<<Relay);
#endif
    
    if (++SecPrescaler >= ProtectTimer1S)
      {
      SecPrescaler = 0;
      if (ProtectTimerOnValue) ProtectTimerOnValue--;
      }
  }
  }
}
