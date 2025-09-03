
#include "statemachineActuator.h"
#ifdef ACTUATOR
void StateMachineActuator::processStateMachine()
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
    state_s1 = STATE_1;

    break;

  case STATE_1:
    state_s1 = STATE_2;
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

    (r == SUCESSO) ? (state_s1 = STATE_4) : (tentativa <= MAX_TENTATIVAS_WIFI ? state_s1 = STATE_2 : state_s1 = INVALID_STATE);
    break;

  case STATE_4:
    utils.sendMessage("[STATEMACHINE] Sending package.", SERIAL_DEBUG, SEM_TOPICO);
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