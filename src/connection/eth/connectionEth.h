
#ifndef _CONNECTION_ETH_H
#define _CONNECTION_ETH_H

#include "../connection.h"

#ifdef ETH_ENABLE
#include <HTTPClient.h>
#include <EthernetENC.h>

#define SPI_HOST 1
#define SPI_CLOCK_MHZ 8
#define INT_GPIO 5

#define MISO_GPIO 12
#define MOSI_GPIO 13
#define SCLK_GPIO 11
#define CS_GPIO 19
// Set the static IP address to use if the DHCP fails to assign
#define MYIPADDR 192, 168, 1, 28
#define MYIPMASK 255, 255, 255, 0
#define MYDNS 192, 168, 1, 1
#define MYGW 192, 168, 1, 1

class ConnectionEth : public Connection
{
public:
  // Variables to measure the speed
  unsigned long beginMicros, endMicros;
  unsigned long byteCount = 0;
  bool printWebData = true; // set to false for better speed measurement
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
  String SSID_SMARTAIR = "SmartAir ";
  String SENHA_SMARTAIR = "12345678";
};

#endif
#endif