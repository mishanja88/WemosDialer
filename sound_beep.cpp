#include "sound_beep.h"
#include <Arduino.h>
#include "eeprom_utils.h"

void sound_init()
{
    pinMode(D4, OUTPUT);
}

void sound_beep(SoundBeep mode)
{
  if(mode != SB_MODE_WIFI && mode != SB_MODE_WORK)
  {
      DeviceSettings settings = eeprom_read_settings();
      if(!settings.isSoundEnabled)
          return;
  }
    
    
  switch (mode)
  {
    case SB_MODE_WIFI:
      {
        tone(D4, 2000, 150);
        delay(150);
        tone(D4, 4000, 150);
        delay(150);
        tone(D4, 6000, 150);
        delay(500);
        tone(D4, 6000, 150);
      }
      break;

    case SB_MODE_WORK:
      {
        tone(D4, 8000, 500);
        delay(100);
        tone(D4, 8000, 500);
      }
      break;

    case SB_MODE_FAILURE:
      {
        tone(D4, 8000, 150);
        delay(150);
        tone(D4, 8000, 150);
        delay(10000);
      }
      break;

    case SB_CANCEL:
      {
        tone(D4, 6000, 150);
        delay(150);
        tone(D4, 4000, 150);
        delay(150);
        tone(D4, 2000, 150);
      }
      break;

    case SB_NUMBER:
      {
        tone(D4, 4000, 200);
      }
      break;

    case SB_DIAL:
      {
          for (int j = 0; j < 3; ++j)
          {
            tone(D4, 4000, 70);
            delay(70);
          }
          delay(500);
      }
      break;
  }
}
