#ifndef __LED_DISPLAY_H
#define __LED_DISPLAY_H

#include "dialer_errno.h"

enum Font
{
  FNT_0,
  FNT_1,
  FNT_2,
  FNT_3,
  FNT_4,
  FNT_5,
  FNT_6,
  FNT_7,
  FNT_8,
  FNT_9,
  
  FNT_DOT,
  FNT_SPACE,
  
  FNT_WIFI_0,
  FNT_WIFI_1,
  
  FNT_SPLASH_0,
  FNT_SPLASH_1,
  
  FNT_ERR,
};

void led_init();

void led_print(Font left, Font right);

void led_print_err(DialerErrno err);

void led_drawString(char* text, int len, int x, int y);

#endif // __LED_DISPLAY_H
