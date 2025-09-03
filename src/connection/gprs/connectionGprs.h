
#ifndef _CONNECTION_GPRS_H
#define _CONNECTION_GPRS_H

#include "../connection.h"
#ifdef GPRS_ENABLE
#define TINY_GSM_RX_BUFFER 650
#define GSM_AUTOBAUD_MIN 1200
#define GSM_AUTOBAUD_MAX 57600
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define GSM_PIN ""
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define TINY_GSM_MODEM_SIM800
#define DEBUGGPRS


class ConnectionGPRS : public Connection
{
public:
  uint8_t setupConn(void) override;
  uint8_t stopConn(void) override;
  uint8_t startConn(void) override;
  uint8_t sendData(String data, String url) override;

private:

};

#endif
#endif