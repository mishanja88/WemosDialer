#ifndef __SOUND_BEEP_H
#define __SOUND_BEEP_H

enum SoundBeep
{
  SB_MODE_WIFI,
  SB_MODE_WORK,
  SB_MODE_FAILURE,
  SB_CANCEL,
  SB_NUMBER,
  SB_DIAL,
};

void sound_beep(SoundBeep mode);

void sound_init();

#endif // __SOUND_BEEP_H
