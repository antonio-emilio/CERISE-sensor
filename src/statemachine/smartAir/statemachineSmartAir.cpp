
#include "statemachineSmartAir.h"
#ifdef SMARTAIR
void StateMachineSmartAir::processStateMachine()
{
  switch (state_s1)
  {

  case AGUARDA_TEMPO:
    LED_STATE = ON_IDLE;

    if (true)
    {
      state_s1 = STATE_0;
      break;
    }

    while (1)
    {
      epochTime = utils.getTime();
      int minute = (epochTime % 3600) / 60;
      utils.sendMessage("[STATEMACHINE] Waiting for sending time (Minute: " + String(minute) + ")", SERIAL_DEBUG, SEM_TOPICO);

      ledState = !ledState;
      digitalWrite(GREEN_LED, ledState);
      digitalWrite(RED_LED, ledState);

      if (minute % 2 == 0)
      {
        utils.sendMessage("[STATEMACHINE] Sampling time! (Minute: " + String(minute) + ")", SERIAL_DEBUG, SEM_TOPICO);
        LED_STATE = WORKING;
        state_s1 = STATE_0;
        break;
      }

      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    break;

  case STATE_0:
    LED_STATE = WORKING;

    leituras = sensors.getSensors();
    r = (leituras["SUCESSO"] == true) ? SUCESSO : ERRO;
    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_1) : (tentativa <= MAX_TENTATIVA_LEITURA_SENSOR ? state_s1 = STATE_0 : state_s1 = MALFUNCIONAMENTO_SENSOR);

    break;

  case STATE_1:
    utils.sendMessage("[STATEMACHINE] Building package.", SERIAL_DEBUG, SEM_TOPICO);

    pacote = utils.createPackage(leituras);

    utils.sendMessage("[STATEMACHINE] Package built successfully.", SERIAL_DEBUG, SEM_TOPICO);
    r = SUCESSO;

    verifyState(r, errCount);
    (r == SUCESSO) ? (state_s1 = STATE_2) : (state_s1 = STATE_1);
    break;

  case STATE_2:
    utils.sendMessage("[STATEMACHINE] Starting WiFi connection verification.", SERIAL_DEBUG, SEM_TOPICO);

    r = connection->setupConn();

#ifdef MQTT_ENABLE
    utils.sendMessage("[STATEMACHINE] Starting MQTT connection verification.", SERIAL_DEBUG, SEM_TOPICO);
    r = mqtt.verifyConn();
#endif

    retorno = (r == SUCESSO) ? "[STATEMACHINE] Connection established successfully" : "[STATEMACHINE] Failed to connect to WiFi (" + String(tentativa) + "/" + String(MAX_TENTATIVAS_WIFI) + ")";
    utils.sendMessage(retorno, SERIAL_DEBUG, SEM_TOPICO);

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_3) : (tentativa <= MAX_TENTATIVAS_WIFI ? state_s1 = STATE_2 : state_s1 = INVALID_STATE);
    break;

  case STATE_3:
    utils.sendMessage("[STATEMACHINE] Verifying packages with sending errors", SERIAL_DEBUG, SEM_TOPICO);

    r = utils.sendNVSpackages();

    retorno = (r == SUCESSO) ? "[STATEMACHINE] Verification completed successfully" : "[STATEMACHINE] Failed to execute verification (" + String(tentativa) + "/" + String(MAX_TENTATIVA_ENVIO) + ")";
    utils.sendMessage(retorno, SERIAL_DEBUG, SEM_TOPICO);

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_4) : (tentativa <= MAX_TENTATIVA_ENVIO ? state_s1 = STATE_3 : state_s1 = INVALID_STATE);
    break;

  case STATE_4:
    utils.sendMessage("[STATEMACHINE] Sending package.", SERIAL_DEBUG, SEM_TOPICO);

    if (lora_enable)
    {
      r = utils.sendMessage(pacote, LORA_DEBUG, SEM_TOPICO);
    }
    else if (!espnow_habilitado() || espnow_root())
    {
      r = utils.sendMessage(pacote, POST_DEBUG, SERVIDOR);
    }
    else
    {
      r = utils.sendMessage(pacote, ESPNOW_DEBUG, SEM_TOPICO);
    }

#ifdef MQTT_ENABLE
    // TODO: Criar uma função padronizada para isso
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/temperatura"), "{\"value\":" + String(leituras["TEMPERATURA"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/umidade"), "{\"value\":" + String(leituras["UMIDADE"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/pressao"), "{\"value\":" + String(leituras["PRESSAO"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/airquality"), "{\"value\":" + String(leituras["GAS"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/luminosidade"), "{\"value\":" + String(leituras["LUX"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/co2"), "{\"value\":" + String(leituras["CO2"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/ruido"), "{\"value\":" + String(leituras["RUIDO"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/nox"), "{\"value\":" + String(leituras["NOX"]) + "}");
    mqtt.sendMessage(PREFIX_MQTT + String("sensor/voc"), "{\"value\":" + String(leituras["VOC"]) + "}");
#endif

    utils.sendMessage("[STATEMACHINE] Sending completed.", SERIAL_DEBUG, SEM_TOPICO);
    qntdLeituras++;
    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_5) : (tentativa <= MAX_TENTATIVA_ENVIO ? state_s1 = STATE_4 : state_s1 = STATE_7);
    break;

  case STATE_5:
    utils.verifyGeneralState(qntdLeituras);
    r = SUCESSO;

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_6) : (state_s1 = STATE_5);
    break;

  case STATE_6:
#ifdef LOW_POWER_ENABLE
    utils.hibernate();
#endif

    utils.waitNextSendTime();
    r = SUCESSO;

    if (shouldReboot){
      ESP.restart();
    }

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = AGUARDA_TEMPO) : (state_s1 = STATE_6);
    break;

  case STATE_7:
    utils.storePackageNVS(pacote);
    if (low_power_enable)
    {
      utils.hibernate();
    }

    utils.waitNextSendTime();
    r = SUCESSO;

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_0) : (state_s1 = STATE_0);
    break;

  case MALFUNCIONAMENTO_SENSOR:
    r = utils.sendMessage("{\"ID\":\"" + String(smartAir_id) + "\",\"tipo\":2,\"INFO\":\"Malfunctioning sensors detected.\"}", POST_DEBUG, SERVIDOR);

    verifyState(r, errCount);

    retorno = (r == SUCESSO) ? ("[STATEMACHINE] Error message sent successfully.") : ("[STATEMACHINE] Failed to send error message.");
    utils.sendMessage(retorno, SERIAL_DEBUG, SEM_TOPICO);

    state_s1 = INVALID_STATE;
    break;

  default:
    utils.cleanPackages();
    ESP.restart();
    break;
  }
}
#endif