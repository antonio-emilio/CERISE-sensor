
#ifndef _STATEMACHINE_ACTUATOR_H
#define _STATEMACHINE_ACTUATOR_H

#include "../statemachine.h"
#ifdef ACTUATOR
class StateMachineActuator : public StateMachine
{
public:

  void processStateMachine(void) override;


private:

};

#endif
#endif