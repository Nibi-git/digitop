switch (code_state)
{
case DN_SHORT:
  switch (DisplayMode)
  {
  //case RealtimeVoltage:                       break;
  case UpTresholdVoltage:                     break;
  case DownTresholdTune:              break;
  case UpTresholdTune:                break;
  case ProtectTimerTune:        break;
  case ProtectTimer:                          break;
  }
break;

case UP_SHORT:
  switch (DisplayMode)
  {
  case RealtimeVoltage:                         break;
  case DownTresholdVoltage:                     break;
  case DownTresholdTune:              break;
  case UpTresholdTune:                break;
  case ProtectTimerTune:        break;
  case ProtectTimer:                            break;  
  }
break;

case DN_MID:
if (DisplayMode == DownTresholdVoltage) 
break;

case UP_MID:
if (DisplayMode == UpTresholdVoltage) 
break;

case DN_UP_MID:
  switch (DisplayMode)
  {
  case DownTresholdVoltage:           break;
  case UpTresholdVoltage:           break;
  }
break;

}
//-----------------------------------------------------------------------------------------

switch (DisplayMode)
{
case RealtimeVoltage:
  switch(code_state)
  {
  case DN_SHORT: DisplayMode = DownTresholdVoltage;
  break;
  case UP_SHORT: DisplayMode = UpTresholdVoltage;
  break;
  case DN_MID:
  break;
  case UP_MID:
  break;
  case DN_UP_MID:
  break;
  }
break;
case UpTresholdVoltage: 
  switch(code_state)
  {
  case DN_SHORT: DisplayMode = DownTresholdVoltage;
  break;
  case UP_SHORT:
  break;
  case DN_MID:
  break;
  case UP_MID: DisplayMode = UpTresholdTune;
  break;
  case DN_UP_MID: DisplayMode = ProtectTimerTune;
  break;
  }
break;
case DownTresholdVoltage: 
  switch(code_state)
  {
  case DN_SHORT: DownTreshold --; CheckTresholdSettings ();
  break;
  case UP_SHORT: DisplayMode = UpTresholdVoltage;
  break;
  case DN_MID: DisplayMode = DownTresholdTune;
  break;
  case UP_MID:
  break;
  case DN_UP_MID: DisplayMode = ProtectTimerTune;
  break;
  }
break;
case UpTresholdTune: 
  switch(code_state)
  {
  case DN_SHORT: UpTreshold --;   CheckTresholdSettings ();
  break;
  case UP_SHORT: UpTreshold ++;   CheckTresholdSettings ();
  break;
  case DN_MID:
  break;
  case UP_MID:
  break;
  case DN_UP_MID:
  break;
  }
break;
case DownTresholdTune: 
  switch(code_state)
  {
  case DN_SHORT:
  break;
  case UP_SHORT: DownTreshold ++; CheckTresholdSettings ();
  break;
  case DN_MID:
  break;
  case UP_MID:
  break;
  case DN_UP_MID:
  break;
  }
break;
case ProtectTimerTune:
  switch(code_state)
  {
  case DN_SHORT: ProtectTimerMaxValue -=5; CheckTimerSettings ();
  break;
  case UP_SHORT: ProtectTimerMaxValue +=5; CheckTimerSettings ();
  break;
  case DN_MID:
  break;
  case UP_MID:
  break;
  case DN_UP_MID:
  break;
  }
break;
case ProtectTimer:
  switch(code_state)
  {
  case DN_SHORT: DisplayMode = DownTresholdVoltage;
  break;
  case UP_SHORT: DisplayMode = UpTresholdVoltage;
  break;
  case DN_MID:
  break;
  case UP_MID:
  break;
  case DN_UP_MID:
  break;
  }
break;
}

