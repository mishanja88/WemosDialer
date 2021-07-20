#include "port.h"
#include "eeprom_utils.h"

#define PORT_BUFFER_LIMIT 500
char g_portBuffer[ PORT_BUFFER_LIMIT + 1];
int g_portBufferLength;

char* port_get_buffer()
{
  return g_portBuffer;
}

int  port_get_buffer_length()
{
  return g_portBufferLength;
}

void port_clear_buffer()
{
  Serial.flush();

  g_portBufferLength = 0;
  memset(g_portBuffer, 0, PORT_BUFFER_LIMIT + 1);
}


void port_update_buffer(unsigned long timeout)
{
  unsigned long time = millis();

  while ((millis() - time) < timeout)
    while(Serial.available())
    {
    char chr(Serial.read());

    if (g_portBufferLength >= PORT_BUFFER_LIMIT)
    {
      g_portBufferLength = 0;

      // removing first line (maybe whole buffer if no CR found)
      bool found = false;
      for (int i = 0; i < PORT_BUFFER_LIMIT; ++i)
      {
        if (found)
          g_portBuffer[ g_portBufferLength++ ] = g_portBuffer[i];

        found |= (g_portBuffer[i] == '\n' || g_portBuffer[i] == '\r');
      }

      // end garbage cleaning
      for (int i = g_portBufferLength; i < PORT_BUFFER_LIMIT; ++i)
      {
        g_portBuffer[ i ] = '\0';
      }
    }

    g_portBuffer[ g_portBufferLength++ ] = chr;
  }
}

void port_send(String data)
{
  Serial.println(data);
}

void port_init()
{
  Serial.begin(9600);

  port_clear_buffer();
}

bool port_send_accepted(String cmd, unsigned long timeout)
{
  port_clear_buffer();
  port_send(cmd);
  delay(100);

  unsigned long time = millis();

  while ((millis() - time) < timeout)
  {
    port_update_buffer(50);
    if (strstr(g_portBuffer, "OK") != NULL)
      return true;
  }

  return false;
}

int port_dial(int idx)
{
  if (!port_send_accepted("AT", 300ul))
    return 1;

  if (!port_send_accepted("ATM0", 300ul))
    return 2;

  if (!port_send_accepted("ATH", 3000ul))
    return 3;

  String dialCmd = eeprom_read_dialable(idx);
  if (!port_send_accepted(dialCmd, 1000ul * (unsigned long)dialCmd.length()))
    return 4;

  // port_update_buffer(1000ul * (unsigned long)dialCmd.length());

  if (!port_send_accepted("ATH", 3000ul))
    return 5;

  return 0;
}
