#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "BEvent.h"

class Buratino;
typedef BEvent<Buratino, void> BTask;

struct TaskPriority
{
  static const int High = 0;
  static const int Medium = 1;
  static const int Low = 2;
};

class BTaskSwitcher {	
protected:
  BTaskSwitcher();
  void Setup(int tasks);
  void Start();
  int RunTask(BTask& delegate, BTask::ArgumentType* arg, uint8_t priority, uintptr_t stackSize);
  void YieldTask();
  void KillTask(int id);
  int CurrentTask();
  friend class Buratino;
};

#endif