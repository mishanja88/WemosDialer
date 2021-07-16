#include <EEPROM.h>

#include "Wire.h"

#include "wifi_mode.h"
#include "sound_beep.h"
#include "led_display.h"
#include "i2c_keypad.h"

#define I2C_KEYPAD_ADDRESS 0x20
i2ckeypad keyPad(I2C_KEYPAD_ADDRESS);

uint32_t start, stop;
uint32_t lastKeyPressed = 0;

char last_key = '\0';  // N = Nokey, F = Fail
bool g_is_wifi_mode;

Font g_digits[2];
int g_digit_index = 0;

enum State {
  S_INIT,
  S_DIGIT,
  S_DIAL
};

State g_state;
uint32_t g_state_millis;

void set_state(State next)
{
  g_state = next;
  g_state_millis = millis();
}

void setup() {
  led_init();
  g_is_wifi_mode = wifi_is_enabled();
  EEPROM.begin(4096);  //Initialize EEPROM

  if (g_is_wifi_mode)
  {
    wifi_setup();
    sound_beep(SB_MODE_WIFI);
    led_print( FNT_WIFI_0, FNT_WIFI_1 );
  }
  else
  {
    wifi_disable();
    sound_beep(SB_MODE_WORK);
    set_state(S_INIT);
  }

  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(I2C_KEYPAD_ADDRESS);
  if (Wire.endTransmission() != 0)
  {
    led_print( FNT_ERR, FNT_0 );

    while (1)
    {
      sound_beep(SB_MODE_FAILURE);
    }
  }

  keyPad.init();
  last_key = keyPad.get_key();
}

char pStrNum = '0';

/*
  NO CARRIER
  NO DIALTONE
  BUSY
  RING
  OK

  выкл динамик
  ATM0

  ATDP 8W9219292306;
  ATDT 8W9219292306;
  ATH

*/

void loop()
{
  if (g_is_wifi_mode)
  {
    wifi_loop();
    return;
  }

  uint32_t now = millis();
  if (now - g_state_millis > 30000)
    set_state(S_INIT);

  switch (g_state)
  {
    case S_INIT:
      {
        g_digits[0] = g_digits[1] = FNT_SPACE;
        g_digit_index = 0;

        led_print( FNT_SPACE, FNT_SPACE );
        set_state(S_DIGIT);
      }
      break;

    case S_DIGIT:
      {
        if (g_digit_index < 2) // blinking dot
        {
          Font isBlink = ((now / 500) & 1) ? FNT_DOT : FNT_SPACE;
          if (g_digits[ g_digit_index ] != isBlink)
          {
            g_digits[ g_digit_index ] = isBlink;
            led_print( g_digits[0], g_digits[1]);
          }
        }

        char key = keyPad.get_key();
        if (key == last_key)
           return;
  
        last_key = key;

        if(key == '\0')
           return;
        
        if (key == 'N' || // cancel by user
            //abs(millis() - g_state_time) > STATE_TIMEOUT_MILLIS || // cancel by timeout
            (key == 'Y' && g_digit_index == 0) ||  // dial, but no digits entered
            (key != 'Y' && g_digit_index >= 2)  // number, but all digits entered
           )
        {
          sound_beep( SB_CANCEL );
          set_state( S_INIT );
          return;
        }

        if (key == 'Y')
        {
          set_state( S_DIAL );
          return;
        }

        g_digits[ g_digit_index ] = (Font)( key - '0');
        led_print( g_digits[0], g_digits[1]);
        g_digit_index++;

        sound_beep( SB_NUMBER );
      }
      break;

    case S_DIAL:
    {
        sound_beep( SB_DIAL );
        set_state(S_INIT);
    }
    break;
  }
/*
  char key = keyPad.get_key();

  if (key != last_key)
  {
    last_key = key;

    if (key != '\0')
    {
      tone(D4, 4000, 100);
      tone(D4, 8000, 100);
      if (key == 'Y' || key == 'N')
        key = FNT_DOT;
      else
        key -= '0';
      // drawString(&key, 1, x, 0);
      // lmd.display();
      led_print( (Font) key, FNT_DOT);
    }
  }

*/
  /*

    // Draw the text to the current position
    int len = strlen(text);
    drawString(text, len, x, 0);
    // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background...

    // Toggle display of the new framebuffer
    lmd.display();

    // Wait to let the human read the display
    delay(ANIM_DELAY);

    // Advance to next coordinate
    if( --x < len * -8 ) {
      x = LEDMATRIX_WIDTH;
    }
  */
}
