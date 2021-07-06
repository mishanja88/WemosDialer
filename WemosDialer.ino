int g_state = 0;
long g_state_time = 0;

int g_digit = 0;
char g_display[2];
bool g_isBlink = false;

void set_state(int state)
{
    g_state = state;
    g_state_time = millis();
}

void enter_state_error( int errCode )
{
    sound_beep( BEEP_ERROR );
    led_print( FNT_ERR, errCode );
    
    cli();
    sleep_enable();
    sleep_cpu();
}

void setup()
{   
    led_init();
    
    if(!init_keyboard())
        enter_state_error(0);
        
    led_print( FNT_SPLASH_0, FNT_SPLASH_1 );
    delay(1000);
    
    g_state = STATE_CHECK_WIFI_SWITCH;
}


void loop()
{
    switch(g_state)
    {
        case STATE_CHECK_WIFI_SWITCH:
        {
            if(is_wifi_mode_on())
            {
                init_network();
                led_print( FNT_WIFI_0, FNT_WIFI_1 );
                set_state( STATE_WIFI );
            }
            else
            {
                set_state( STATE_DIGIT );
                g_digit = 0;
                g_display[0] = FNT_SPACE;
                g_display[1] = FNT_SPACE;
                led_print( FNT_SPACE, FNT_SPACE );
            }
            sound_beep( BEEP_READY );            
        }
        break;
        
        case STATE_WIFI:
        {
            if(keyboard_pressed_key() == BTN_CANCEL)
            {
                deinit_network();
                sound_beep( BEEP_CANCEL );
                set_state( STATE_CHECK_WIFI_SWITCH );
            }
        }
        break;
            
        case STATE_DIGIT:
        {
            if(g_digit < 2) // blinking dot
            {
                bool isBlink = (millis() / 500) & 1;
                if(isBlink != g_isBlink)
                {
                    g_isBlink = isDot;
                    g_display[ g_digit ] = isDot ? FNT_DOT : FNT_SPACE;
                    led_print( g_display[0], g_display[1]);
                }
            }
            
            int key = keyboard_pressed_key();
            if(key == BTN_CANCEL || // cancel by user
                abs(millis() - g_state_time) > STATE_TIMEOUT_MILLIS || // cancel by timeout
                (key == BTN_OK && g_digit == 0) ||  // dial, but no digits entered   
                (key != BTN_OK && g_digit >= 2)  // number, but all digits entered   
            )              
            {
                sound_beep( BEEP_CANCEL );
                set_state( STATE_CHECK_WIFI_SWITCH );
                return;
            }
            
            if(key == BTN_NONE)
                return;
            
            if(key == BTN_OK)
            {
                set_state( STATE_DIAL );
                return;
            }
            
            g_display[ g_digit ] = key;
            led_print( g_display[0], g_display[1]);
            g_digit++;   
            
            sound_beep( BEEP_NUMBER );
        }    
        break;
            
        case STATE_DIAL:
        {
            int num = g_display[0];
            if(g_digit == 2)
                num = num*10 + g_display[1];

            sound_beep( BEEP_DIAL );
            
            int errCode = dial_by_memory_index( num );
            if(errCode)
                enter_state_error( errCode );
            else
                set_state( STATE_CHECK_WIFI_SWITCH );
        }
        break;        
        
        default: break;
    }    
}
