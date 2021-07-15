#include "sound_beep.h"
#include <Arduino.h>

void sound_beep(SoundBeep mode)
{
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
        for (int i = 0; i < 3; ++i)
        {
          for (int j = 0; j < 3; ++j)
          {
            tone(D4, 4000, 150);
            delay(150);
          }
          delay(500);
        }
      }
      break;
  }
}
