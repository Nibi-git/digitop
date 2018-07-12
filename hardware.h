#define RELEASE_BUILD
//#define TEST_BUILD_1
//#define TEST_BUILD_2
//#define TEST_BUILD_3

//готовые конфиги
//--------------------------
#ifdef RELEASE_BUILD
#define METHOD_AR //средневыпрямленное
#define SMOOTH_MEASURE

#define SmoothShift 3 // сдвиг при делении
#define SmoothBufferSize 8 // соответствующий сдвигу размер буфера

//#define OVERRIDE_SETTINGS
#define OVERRIDE_K1_VALUE 780
#define OVERRIDE_UP_TRESHOLD_VALUE 260
#define OVERRIDE_DOWN_TRESHOLD_VALUE 160
#define OVERRIDE_PROTECT_TIMER_MAX_VALUE 120

//#define KEYBOARD_DISABLE

#endif
//--------------------------
#ifdef TEST_BUILD_1
//#define METHOD_AR
#define METHOD_RMS //среднеквадратическое
#define KEYBOARD_DISABLE

#define OVERRIDE_SETTINGS
#define OVERRIDE_K1_VALUE 780
#define OVERRIDE_UP_TRESHOLD_VALUE 260
#define OVERRIDE_DOWN_TRESHOLD_VALUE 160
#define OVERRIDE_PROTECT_TIMER_MAX_VALUE 120
#endif
//--------------------------
#ifdef TEST_BUILD_2
#define METHOD_PEAK
#endif
//--------------------------
#ifdef TEST_BUILD_3
#define METHOD_DIRECT_SHOW
#endif
//--------------------------

#define CtrlClockRate 8000000

#define PortAnode PORTA
#define PortAnodeDir DDRA

#define PortKathode PORTB
#define PortKathodeDir DDRB

#define PortKathodeAndRelayDir DDRB // маразм крепчал

#define Kathode_1 PB1
#define Kathode_2 PB0
#define Kathode_3 PB6

#define PortButton PORTB
#define PortButtonDir DDRB
#define PinButton PINB

#define Button_DN 2
#define Button_UP 5

#define PortRelay PORTB
#define PortRelayDir DDRB

#define Relay 3
#define InADC 7

#define SEG_A PA0
#define SEG_B PA7
#define SEG_C PA5
#define SEG_D PA3
#define SEG_E PA2
#define SEG_F PA1
#define SEG_G PA4
#define SEG_DP PA6

//#define T0_RELOAD 0x83 //200 83
#define T1_RELOAD 0xb5 //128 samples 78us

#define NUMBER_DIGITS 3

#define shortpress 1
#define midpress 120 //80
#define longpress 250 //150
#define KeyTimerMax 500

//code_state
#define DN_SHORT 1
#define UP_SHORT 2
#define DN_MID 3
#define UP_MID 4
#define DN_UP_MID 5

//DisplayMode
#define RealtimeVoltage 1
#define UpTresholdVoltage 2
#define DownTresholdVoltage 3
#define ProtectTimer 4

#define UpTresholdTune 50
#define DownTresholdTune 51
#define ProtectTimerTune 52
#define K1Tune 53

/*
775-b2
780-b5
785-b7
*/

//settings validate
#define HALF_BYTE_CONST 128
#define UP_TRESHOLD_MAX 290
#define UP_TRESHOLD_MIN 210
#define DOWN_TRESHOLD_MAX 200
#define DOWN_TRESHOLD_MIN 140
#define PROTECT_TIMER_ON_MAX 995
#define PROTECT_TIMER_ON_MIN 5
#define PROTECT_TIMER_STEP 5
#define K1_MAX 999
#define K1_MIN 1

#define DOWN_TRESHOLD_FAST_MIN 120 // в вольтах

#define DACNoiseTreshold 0//20
#define HalfWaveMeasureLength 128
#define RMSMeasureLength HalfWaveMeasureLength/2 // интегрируем только половину полуволны, вторая испорчена собственным источником питания

#define ProtectTimer1S 200
#define ProtectTimerOffMaxValue ProtectTimer1S*1

//АЦП
//100 семплов за полуволну 100 мкс интервал
//128 - 78,125 мкс
//ацп
//8000/16=500 кгц Tконв= 26 мкс
//8000/32=250 кгц Tконв= 52 мкс // достаточно
//8000/64=125 кгц Tконв= 104 мкс

//5 << ADPS0 Prescaler = 32  f = 8000000/(32*13)=19230 Гц
//6 << ADPS0 Prescaler = 64  f = 8000000/(64*13)=9615 Гц
//7 << ADPS0 Prescaler = 128

//http://arduino.ru/forum/proekty/voltmetr-peremennogo-napryazheniya
//http://cxem.net/izmer/izmer90.php
//http://catcatcat.d-lan.dp.ua/obuchenie/izuchaem-pic24-kompilyator-xc16/10-bit-vyisokoskorostnoy-analogo-tsifrovoy-preobrazovatel-chast-2/
