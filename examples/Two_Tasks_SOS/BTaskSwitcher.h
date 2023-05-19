#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "BEvent.h"

class Buratino;
typedef BEvent<Buratino, void> BTask;

struct BTaskInfo{
  uint8_t* sp; // needs to be aligned
  int id;
  BTask::ArgumentType* arg;
  BTask delegate;
  uint8_t stack[1];
};

class BTaskSwitcher {	
protected:
  BTaskSwitcher();
  void Setup(int tasks);
  void Start();
  int RunTask(BTask& delegate, BTask::ArgumentType* arg, uintptr_t stackSize);
  void YieldTask();
  void KillTask(int id);
  int CurrentTask();
  friend class Buratino;
};

#endif