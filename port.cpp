#include "port.h"
#include "eeprom_utils.h"
#include "dialer_errno.h"

#define PORT_BUFFER_LIMIT 1000
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

DialerErrno port_check_accepted()
{
    const char *failCodes[] =
    {
       "NO CARRIER",
       "NO DIALTONE",
       "BUSY",
       "ERROR",
       NULL
    };

    for(int i = 0; failCodes[i] != NULL; ++i)
      if(strstr(g_portBuffer, failCodes[i]) != NULL)
         return (DialerErrno)(ERR_MODEM_NO_CARRIER + i);
         
    if(strstr(g_portBuffer, "OK") != NULL)
       return ERR_NONE;

   return ERR_MODEM_TIMEOUT;
}


DialerErrno port_send_accepted(String cmd, unsigned long timeout)
{
  port_clear_buffer();
  port_send(cmd);

  for(unsigned long t = 0; t < timeout / 100; ++t)
  {
    port_update_buffer(100);
    DialerErrno retCode = port_check_accepted();
    if(retCode != ERR_MODEM_TIMEOUT)
       return retCode;
  }

// return ERR_NONE;
  return ERR_MODEM_TIMEOUT;
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

String logTime(unsigned long start)
{
  return String("\n ms: ") + String(millis()- start, DEC) + "\n";
}

void port_dial_debug_to_buffer(int idx)
{
  String result = port_dial_debug(idx);

  strncpy(g_portBuffer, result.c_str(), PORT_BUFFER_LIMIT);
  g_portBufferLength = min(result.length(), (unsigned int) PORT_BUFFER_LIMIT);
}

String port_dial_debug(int idx)
{
  DialerErrno ret;
  String result;
  unsigned long start = millis();
  
  ret = port_send_accepted("AT", 3000ul);
  if (ret)
    return result + logTime(start) + "Err 1\n" + errnoToString(ret) + "\n" + g_portBuffer;

  result += logTime(start) + g_portBuffer;

  ret = port_send_accepted("ATM0", 3000ul);
  if (ret)
    return result + logTime(start) + "Err 2\n" + errnoToString(ret) + "\n" + g_portBuffer;

  result += logTime(start) + g_portBuffer;
  
  ret = port_send_accepted("ATH", 3000ul);
  if (ret)
    return result + logTime(start) + "Err 3\n" + errnoToString(ret) + "\n" + g_portBuffer;

  result += logTime(start) + g_portBuffer;


  String dialCmd = eeprom_read_dialable(idx);
  ret = port_send_accepted(dialCmd, 10000ul * (unsigned long)dialCmd.length());
  if (ret)
    return result + logTime(start) + "Err 4\n" + errnoToString(ret) + "\n" + g_portBuffer;

  result += logTime(start) + g_portBuffer;

  port_update_buffer(10000ul);

  ret = port_send_accepted("ATH", 3000ul);
  if (ret)
    return result + logTime(start) + "Err 5\n" + errnoToString(ret) + "\n" + g_portBuffer;

  result += logTime(start) + g_portBuffer + "\n OK!!!";

  return result;
}
