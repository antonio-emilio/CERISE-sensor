#ifndef _STATEMACHINE_SMARTGATEWAY_H
#define _STATEMACHINE_SMARTGATEWAY_H

#include "../statemachine.h"


#ifdef SMARTGATEWAY
class StateMachineSmartGateway : public StateMachine
{
public:

  void processStateMachine(void) override;
  void setupStateMachine(void) override;


private:

};

#endif
#endif
