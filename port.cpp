#include "port.h"

String g_portBuffer;

String port_get_buffer()
{
  return g_portBuffer;
}

void port_update_buffer()
{
  while(Serial.available())
    g_portBuffer += char(Serial.read());
}

void port_send(String data)
{
  Serial.println(data);
}

void port_init()
{
  Serial.begin(9600);
  Serial.flush();
}
