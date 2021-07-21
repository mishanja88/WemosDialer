#ifndef __PORT_H
#define __PORT_H

#include <Arduino.h>
#include "dialer_errno.h"

char* port_get_buffer();

int  port_get_buffer_length();

void port_clear_buffer();

void port_update_buffer(unsigned long timeout);

DialerErrno port_send_accepted(String cmd, unsigned long timeout);

void port_send(String data);

void port_init();

int port_dial(int idx);

void port_dial_debug_to_buffer(int idx);

String port_dial_debug(int idx);

#endif // __PORT_H
