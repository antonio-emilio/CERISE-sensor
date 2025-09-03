#include "statemachineSmartGateway.h"

#ifdef SMARTGATEWAY
void StateMachineSmartGateway::setupStateMachine()
{
  utils.configureSerial();
  utils.sendMessage("[STATEMACHINE] Starting SmartGateway.", SERIAL_DEBUG, SEM_TOPICO);

  task.configureTasks();
  utils.getNVSvalues();
  configureConnection();
  configurePowerMode();
  sensors.configurePins();
  sensors.configureSensors();
  connection->setupConn();
  startProcesses();

  utils.configureNTP();
  LED_STATE = WORKING;

#ifdef MQTT_ENABLE
  mqtt.startConn();
#endif

  utils.sendMessage("[STATEMACHINE] SmartGateway started.", SERIAL_DEBUG, SEM_TOPICO);
}

void StateMachineSmartGateway::processStateMachine()
{
  switch (state_s1)
  {
  case STATE_0:
    if (packageAvailable)
    {
      packageAvailable = false;
      state_s1 = STATE_1;
    }
    else
    {
      state_s1 = STATE_0;
    }

    break;

  case STATE_1:
    utils.sendNVSpackages();
    state_s1 = STATE_0;

    break;

  default:
    ESP.restart();
    break;
  }
}
#endif