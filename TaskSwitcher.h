#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "Bind.h"
#include "Buratino.h"

class TaskSwitcher;
struct TaskInfo;

class Task {
public:
  typedef EventDelegate<Buratino, void> Delegate;
};

struct TaskInfoBase {
  int8_t id;
  Task::TaskDelegate::Argument* arg;
  Task::TaskDelegate delegate;
  TaskInfoBase();
};

class TaskSwitcher {
protected:
  void Setup();
  void RunTask(Task::TaskDelegate& delegate, Task::TaskDelegate::Argument* arg, uint16_t stackSize);

protected:
  friend class Buratino;
};

#endif