#include "statemachineVapeDetect.h"
#ifdef VAPE_DETECTOR

void StateMachineVapeDetect::processStateMachine()
{
  switch (state_s1)
  {

  case STATE_0:
    utils.sendMessage("[STATEMACHINE] Starting WiFi connection verification.", SERIAL_DEBUG, SEM_TOPICO);

    r = connection->setupConn();

    retorno = (r == SUCESSO) ? "[STATEMACHINE] Connection established successfully" : "[STATEMACHINE] Failed to connect to WiFi (" + String(tentativa) + "/" + String(MAX_TENTATIVAS_WIFI) + ")";
    utils.sendMessage(retorno, SERIAL_DEBUG, SEM_TOPICO);

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_3) : (tentativa <= MAX_TENTATIVAS_WIFI ? state_s1 = STATE_0 : state_s1 = INVALID_STATE);
    break;

  case STATE_3:
    utils.verifyGeneralState(qntdLeituras);
    r = SUCESSO;

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_4) : (state_s1 = INVALID_STATE);
    break;

  case STATE_4:
    utils.sendMessage("[STATEMACHINE] Building package.", SERIAL_DEBUG, SEM_TOPICO);
    leituras = sensors.getSensors();
    pacote = utils.createPackage(leituras);

    utils.sendMessage("[STATEMACHINE] Package built successfully.", SERIAL_DEBUG, SEM_TOPICO);
    r = SUCESSO;

    verifyState(r, errCount);
    (r == SUCESSO) ? (state_s1 = STATE_5) : (state_s1 = STATE_0);
    break;

  case STATE_5:
    utils.sendMessage("[STATEMACHINE] Starting WiFi connection verification.", SERIAL_DEBUG, SEM_TOPICO);

    r = connection->setupConn();

    retorno = (r == SUCESSO) ? "[STATEMACHINE] Connection established successfully" : "[STATEMACHINE] Failed to connect to WiFi (" + String(tentativa) + "/" + String(MAX_TENTATIVAS_WIFI) + ")";
    utils.sendMessage(retorno, SERIAL_DEBUG, SEM_TOPICO);

    verifyState(r, errCount);

    (r == SUCESSO) ? (state_s1 = STATE_6) : (tentativa <= MAX_TENTATIVAS_WIFI ? state_s1 = STATE_0 : state_s1 = INVALID_STATE);
    break;

  case STATE_6:
    utils.sendMessage("[STATEMACHINE] Sending package.", SERIAL_DEBUG, SEM_TOPICO);

    r = utils.sendMessage(pacote, POST_DEBUG, SERVIDOR);

    utils.sendMessage("[STATEMACHINE] Sending completed.", SERIAL_DEBUG, SEM_TOPICO);
    qntdLeituras++;
    verifyState(r, errCount);

    utils.waitNextSendTime();
    ESP.restart();

    (r == SUCESSO) ? (state_s1 = STATE_0) : (tentativa <= MAX_TENTATIVA_ENVIO ? state_s1 = STATE_0 : state_s1 = INVALID_STATE);
    break;

  default:
    ESP.restart();
    break;
  }
}

#endif
