#include "dialer_errno.h"


String errnoToString(DialerErrno err)
{
  const char *errStr[] = {
  "ERR_NONE",
  "ERR_KEYBOARD_TIMEOUT",
  "ERR_MODEM_TIMEOUT",
  "ERR_MODEM_NO_CARRIER",
  "ERR_MODEM_NO_DIALTONE",
  "ERR_MODEM_BUSY",
  "ERR_MODEM_UNKNOWN",
  };

  return errStr[err];
};
