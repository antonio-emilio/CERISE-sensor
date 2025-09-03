#ifndef _STATEMACHINE_VAPEDETECT_H
#define _STATEMACHINE_VAPEDETECT_H

#include "../statemachine.h"

#ifdef VAPE_DETECTOR
class StateMachineVapeDetect : public StateMachine
{
public:

  void processStateMachine(void) override;


private:

};

#endif
#endif