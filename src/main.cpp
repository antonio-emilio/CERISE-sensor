#include "Arduino.h"
#include "statemachine/statemachine.h"
#include "statemachine/smartAir/statemachineSmartAir.h"
#include "statemachine/vapeDetect/statemachineVapeDetect.h"
#include "statemachine/smartGateway/statemachineSmartGateway.h"
#include "statemachine/smartBait/statemachineSmartBait.h"
#include "statemachine/actuator/statemachineActuator.h"

StateMachine *stateMachine;

/**
 * Faz as configurações iniciais do sistema
 * @param void sem parametros
 * @return void sem retorno
 */
void setup(void)
{
#ifdef SMARTAIR
  stateMachine = new StateMachineSmartAir();
#elif defined VAPE_DETECTOR
  stateMachine = new StateMachineVapeDetect();
#elif defined SMARTGATEWAY
  stateMachine = new StateMachineSmartGateway();
#elif defined SMARTBAIT
  stateMachine = new StateMachineSmartBait();
#elif defined ACTUATOR
  stateMachine = new StateMachineActuator();
#else
  stateMachine = new StateMachine();
#endif
  stateMachine->setupStateMachine();
}

/**
 * Laço principal do sistema
 * @param void sem parametros
 * @return void sem retorno
 */
void loop(void)
{
  stateMachine->processStateMachine();
}