#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "Bind.h"

class Buratino;
typedef EventDelegate<Buratino, void> BTask;

struct TaskInfoBase {
  int8_t id;
  BTask::Argument* arg;
  BTask delegate;
  TaskInfoBase();
};

class TaskSwitcher {
protected:
  TaskSwitcher();
  void Setup(int8_t tasks);
  void RunTask(BTask delegate, BTask::Argument* arg, uint16_t stackSize);
  void YieldTask();
  void KillTask(int8_t id);
  friend class Buratino;
};

#endif