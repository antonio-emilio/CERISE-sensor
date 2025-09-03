#include "statemachineSmartBait.h"
#ifdef SMARTBAIT
#include "driver/adc.h"
#include <esp_bt.h>

void waitDetection()
{
  uint timeout = millis();

  while (digitalRead(PIR_1) == HIGH || digitalRead(PIR_2) == HIGH || digitalRead(PIR_3) == HIGH)
  {
    delay(1000);
    Serial.println("[STATEMACHINE] Waiting for disarm ...");

    if (millis() - timeout > 4000)
    {
      Serial.println("[STATEMACHINE] Detection timeout.");
      break;
    }
  }
}

void StateMachineSmartBait::setupStateMachine()
{

  uint8_t pkgSend = 0;
  utils.configureSerial();
  utils.sendMessage("[STATEMACHINE] Starting smartBait.", SERIAL_DEBUG, SEM_TOPICO);
  task.configureTasks();
  utils.getNVSvalues();
  configureConnection();
  configurePowerMode();
  sensors.configurePins();

  pacote = utils.createPackage(leituras);
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    pkgSend = 1;
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    pkgSend = 1;
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    connection->setupConn();
    utils.sendHealthCheck();
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    connection->setupConn();
    utils.sendHealthCheck();
    break;
  }

  if (pkgSend)
  {
    nAlerts = nAlerts + 1;
    Serial.println("[STATEMACHINE] Number of alerts after increasing: " + String(nAlerts));
    utils.saveContentParameterNVS(NRO_ALERT, String(nAlerts), TIPO_STRING);

    if (nAlerts > LIMIT_ALERTS)
    {
      Serial.println("[STATEMACHINE] Number of alerts reached the limit. Sending package.");
#ifdef ESPNOW_ENABLE
      startProcesses();
      r = utils.sendMessage(pacote, ESPNOW_DEBUG, SEM_TOPICO);
#else
      connection->setupConn();
#endif
      connection->sendData(pacote, SERVIDOR);

      nAlerts = 0;
      utils.saveContentParameterNVS(NRO_ALERT, String(nAlerts), TIPO_STRING);

      Serial.println("[STATEMACHINE] Number of alerts reseted. Device hill hibernate till next day.");
      utils.hibernate();
    }
  }
  else
  {
    Serial.println("[STATEMACHINE] No alert to send.");
  }

  waitDetection();
  Serial.println("[STATEMACHINE] Starting low power mode (hibernation). Waiting for the next detection.");
  esp_sleep_enable_ext1_wakeup(GPIO_SEL_4 | GPIO_SEL_15 | GPIO_SEL_2, ESP_EXT1_WAKEUP_ANY_HIGH);
  utils.hibernate();
}

void StateMachineSmartBait::processStateMachine()
{
  delay(1000);
}

#endif