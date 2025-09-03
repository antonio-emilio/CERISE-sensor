#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include "Arduino.h"
#include "mesh.h"
#include "globaldef.h"
#include "utils.h"
#include "sensors.h"
#include "task.h"
#include "mqtt.h"
#include "connection/wifi/connectionWifi.h"

extern Connection *connection;

class StateMachine
{
public:
  uint8_t state_s1 = STATE_0;
  uint8_t state_prev_s1;
  uint8_t pin_s1;
  uint8_t val_s1;
  uint8_t r;
  String retorno = "";
  uint8_t tentativa = 0;
  unsigned long epochTime;
  bool ledState = 1;

  /**
   * Inicializa o smartAir chamando todas as funções necessárias
   * @param void sem parametros
   * @return void sem retorno
   */
  virtual void setupStateMachine(void);

  /**
   * Processa os estados do SmartAir
   * @param void sem parametros
   * @return void sem retorno
   */
  virtual void processStateMachine(void);

  /**
   * Registra tentativas de envio
   * @param estado, errCount
   * @return void sem retorno
   */
  void verifyState(uint8_t estado, uint8_t errCount);
  void configureConnection();
  void configurePowerMode();
  void startProcesses();

  Sensors sensors;
  Utils utils;
  Tasks task;
  MQTT mqtt;
  std::map<std::string, float> leituras;
  String pacote;

private:
};

#endif
