#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "BEvent.h"

namespace B {
  uint8_t disable();
  void restore(uint8_t sreg);
}

class Buratino;
typedef BEvent<Buratino, void> BTask;

class BTaskSwitcher {	
protected:
  BTaskSwitcher();
  void Setup(int8_t tasks);
  void Start();
  int8_t RunTask(BTask& delegate, BTask::ArgumentType* arg, uint16_t stackSize);
  void YieldTask();
  void KillTask(int8_t id);
  int8_t CurrentTask();
  friend class Buratino;
};

#endif