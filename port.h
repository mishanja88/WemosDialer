#ifndef __PORT_H
#define __PORT_H

#include <Arduino.h>

char* port_get_buffer();

int  port_get_buffer_length();

void port_clear_buffer();

void port_update_buffer(unsigned long timeout);

void port_send(String data);

void port_init();

int port_dial(int idx);

#endif // __PORT_H
