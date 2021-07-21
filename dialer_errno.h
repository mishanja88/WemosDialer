#ifndef __DIALER_ERRNO_H
#define __DIALER_ERRNO_H

#include <Arduino.h>

enum DialerErrno
{
  ERR_NONE             = 0,
  ERR_KEYBOARD_TIMEOUT = 1,
  ERR_MODEM_TIMEOUT    = 2,
  ERR_MODEM_NO_CARRIER    = 3,
  ERR_MODEM_NO_DIALTONE   = 4,
  ERR_MODEM_BUSY          = 5,
  ERR_MODEM_UNKNOWN       = 6,
};

String errnoToString(DialerErrno err);

#endif // __DIALER_ERRNO_H
