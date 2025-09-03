#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "Arduino.h"
#include "mesh.h"
#include "globaldef.h"
#include "utils.h"
#include "sensors.h"
#include "task.h"
#include "mqtt.h"


class Connection
{
public:
  virtual uint8_t setupConn(void);
  virtual uint8_t verifyConn(void);
  virtual uint8_t startConn(void);
  virtual uint8_t stopConn(void);
  virtual uint8_t restartConn(void);
  virtual uint8_t getConnType(void);
  virtual uint8_t getConnStatus(void);
  virtual uint8_t getConnStrength(void);
  virtual String getIP(void);
  virtual String getSSID(void);
  virtual String getPass(void);
  virtual uint8_t sendData(String data, String url);
  virtual uint8_t resetCredentials(void);

private:
};

#endif
