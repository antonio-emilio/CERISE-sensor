#include "statemachine.h"
Connection *connection;
uint8_t qntdLeituras = 0;
uint8_t errCount = 0;
String dataBoot = "";

void StateMachine::configurePowerMode()
{
#ifdef LOW_POWER_ENABLE || defined(SMARTBAIT)
  Serial.println("[STATEMACHINE] Starting low power mode.");
  setCpuFrequencyMhz(80);
#else
  Serial.println("[STATEMACHINE] Starting high performance mode.");
  setCpuFrequencyMhz(240);
#endif
}

String getDateString()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("[UTILS] Failed to obtain actual time");
    return "";
  }
  String data = String(timeinfo.tm_mday) + "/" + String(timeinfo.tm_mon + 1) + "/" + String(timeinfo.tm_year + 1900) + " " + String(timeinfo.tm_hour - 3) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec);

  return data;
}

void StateMachine::startProcesses()
{

#ifdef SMARTGATEWAY
  configureESPNOWrecv();
  return;
#elif defined(ESPNOW_ENABLE)
  configureESPNOWsend();
  return;
#endif
  dataBoot = getDateString();
}

void StateMachine::configureConnection()
{

#ifdef WIFI_ENABLE
  connection = new ConnectionWiFi();
#elif defined(ETH_ENABLE)
  connection = new ConnectionETH();
#elif defined(LORA_ENABLE)
  connection = new ConnectionLORA();
#elif defined(SMARTBAIT)
  connection = new ConnectionWiFi();
#endif
}

void StateMachine::setupStateMachine()
{
  utils.configureSerial();
  utils.sendMessage("[STATEMACHINE] Starting device.", SERIAL_DEBUG, SEM_TOPICO);

  task.configureTasks();
  utils.getNVSvalues();
  configureConnection();
  configurePowerMode();
  sensors.configurePins();
  sensors.configureSensors();
  connection->setupConn();
  utils.configureNTP();
  startProcesses();

#ifdef MQTT_ENABLE
  mqtt.startConn();
#endif
  utils.sendHealthCheck();
  utils.sendMessage("[STATEMACHINE] Device started.", SERIAL_DEBUG, SEM_TOPICO);
}

void StateMachine::verifyState(uint8_t estado, uint8_t errCount)
{
  if (estado == ERRO)
  {
    tentativa++;
    errCount++;
  }
  else
  {
    tentativa = 0;
  }
}

void StateMachine::processStateMachine()
{
  switch (state_s1)
  {
  default:
    utils.sendMessage("[STATEMACHINE] Não há nenhuma implementação definida.", SERIAL_DEBUG, SEM_TOPICO);
    vTaskDelay(pdMS_TO_TICKS(1000));
    break;
  }
}