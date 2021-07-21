#include <EEPROM.h>

#include "Wire.h"

#include "wifi_mode.h"
#include "sound_beep.h"
#include "led_display.h"
#include "port.h"
#include "i2c_keypad.h"
#include "eeprom_utils.h"
#include "dialer_errno.h"

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

#define ITER_COUNT_DIAL 5
#define ITER_COUNT_INIT 3
#define ITER_ATD_IDX 3

DialerErrno dial_iterations(int iterCount, int idx = 0)
{
    DialerErrno retCode = ERR_NONE;
    
    const char *cmd[ ITER_COUNT_DIAL ] = {
        "AT", "ATM0", "ATH", "ATD...", "ATH"
    };
    
    for(int iter = 0; iter < iterCount; ++iter)
    {
        if(iter == ITER_ATD_IDX)
        {
            String dialCmd = eeprom_read_dialable(idx);
            retCode = port_send_accepted(dialCmd, 10000ul * (unsigned long)dialCmd.length());
            
            if (retCode == ERR_NONE)
            {
                bool needDelay = true;
                for(int blinkCnt = 0; needDelay && blinkCnt < 10; ++blinkCnt)
                {
                    if(blinkCnt & 1)
                        led_print( FNT_SPACE, FNT_SPACE);
                    else
                        led_print( g_digits[0], g_digits[1]);
                    
                    for(int pollCnt = 0; needDelay && pollCnt < 10; ++pollCnt)
                    {
                        port_update_buffer(100ul);
                        
                        char key = keyPad.get_key();
                        if (key == last_key)
                            continue;
                
                        last_key = key;

                        if(key == '\0')
                            continue;
                        
                        sound_beep( SB_CANCEL );
                        needDelay = false;
                    }
                }
            }
        }
        else
        {
            retCode = port_send_accepted(cmd[iter], 3000ul);
        }
    
        if (retCode != ERR_NONE)
        {
            break;
        }
    }
    
    return retCode;
}


void setup() 
{
  EEPROM.begin(4096);  //Initialize EEPROM
  
  sound_init();  
  port_init();  
  led_init();

  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(I2C_KEYPAD_ADDRESS);
  if (Wire.endTransmission() != 0) // keypad check
  {
    while (1) // severe failure, need endless loop
    {
        led_print_err(ERR_KEYBOARD_TIMEOUT);
        sound_beep(SB_MODE_FAILURE);
        led_print(FNT_SPACE, FNT_SPACE);
        delay(200);
    }
  }

  keyPad.init();
  last_key = keyPad.get_key();
  
  g_is_wifi_mode = wifi_is_enabled();
  
  if(! g_is_wifi_mode)
  {
    // Port check in work mode. It's possible to connect modem and pass it
    DialerErrno errCode;
    while(ERR_NONE != (errCode = dial_iterations(ITER_COUNT_INIT)))
    {
        led_print_err(errCode);
        sound_beep(SB_MODE_FAILURE);
        led_print(FNT_SPACE, FNT_SPACE);
        delay(200);      
    }
  }
  
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
}


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

  // inactivity timer  
  uint32_t now = millis();
  if (now - g_state_millis > 30000ul)
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
        set_state(S_DIGIT); // to flush inactivity timer
      }
      break;

    case S_DIAL:
    {
        if(g_digit_index == 1)
        {
            g_digits[1] = FNT_SPACE;
            led_print( g_digits[0], g_digits[1]);
        }
        
        sound_beep( SB_DIAL );
        
        int idx = 0;
        for(int i = 0; i < g_digit_index; ++i)
        {
            idx = 10*idx + g_digits[i];
        }
        
        DialerErrno retCode = dial_iterations(ITER_COUNT_DIAL, idx);        
        
        if(retCode != 0)
        {            
            for(int i = 0; i < 3; ++i)
            {
                led_print_err(retCode);
                sound_beep(SB_MODE_FAILURE);
                led_print(FNT_SPACE, FNT_SPACE);
                delay(200);
            }
        }
        
        set_state(S_INIT);
    }
    break;
  }
}
