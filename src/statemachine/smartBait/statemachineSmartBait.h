#ifndef _STATEMACHINE_SMARTBAIT_H
#define _STATEMACHINE_SMARTBAIT_H

#include "../statemachine.h"
#ifdef SMARTBAIT

class StateMachineSmartBait : public StateMachine
{
public:

  void processStateMachine(void) override;
  void setupStateMachine(void) override;


private:

};

#endif
#endif