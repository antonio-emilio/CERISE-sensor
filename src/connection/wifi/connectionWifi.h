#ifndef _CONNECTION_WIFI_H
#define _CONNECTION_WIFI_H

#include "../connection.h"
#include <HTTPClient.h>

class ConnectionWiFi : public Connection
{
public:
  uint8_t setupConn(void) override;
  uint8_t verifyConn(void) override;
  uint8_t stopConn(void) override;
  uint8_t restartConn(void) override;
  uint8_t getConnType(void) override;
  uint8_t getConnStatus(void) override;
  uint8_t getConnStrength(void) override;
  String getIP(void) override;
  String getSSID(void) override;
  String getPass(void) override;
  uint8_t sendData(String data, String url) override;
  uint8_t resetCredentials(void) override;

private:
  String SSID_SMARTAIR = DEVICE_NAME;
  String SENHA_SMARTAIR = "7412369852";
};

#endif
