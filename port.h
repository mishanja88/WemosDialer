#ifndef __PORT_H
#define __PORT_H

#include <Arduino.h>

String port_get_buffer();

void port_update_buffer();

void port_send(String data);

void port_init();

#endif // __PORT_H
