#ifndef __BTASKSWITCHER_H__
#define __BTASKSWITCHER_H__

#include <new>
#include "BTask.h"
#include "BList.h"
#include "TaskSwitcherSAMD.h"

__BTASKSWITCHER_ARCH_HEADER__

struct TaskPriority {
  static const int High = 0;
  static const int Medium = 1;
  static const int Low = 2;
};

template<typename T>
int runTask(void (*task)(T arg), T arg, uint8_t priority, unsigned stackSize);

class BTaskSwitcher {
protected:
  struct BTaskInfoBase {
    uint8_t* sp;
    int id;
    uint8_t priority;
    virtual ~BTaskInfoBase() {}
  };

  template<typename T>
  struct BTaskInfo : BTaskInfoBase {
    typename BTask<T>::ArgumentType arg;
    BTask<T> delegate;
    BTaskInfo() {}
  };

  struct BSwitchState {
    int current;
    int count;
  };

  typedef void (*BTaskWrapper)(BTaskInfoBase*);

protected:
  static BList<BTaskInfoBase*> _tasks;
  static unsigned _current_task;
  static unsigned _next_task;
  static BSwitchState _pri[3];

protected:
  static int current_task_id();
  static void free_task(int id);
  static int get_next_task();
  static unsigned context_size();
  static bool disable();
  static void restore(bool enable);
  static void initialize(unsigned tasks);
  static void yield_task();
  static void kill_task(int id);
  static void init_arch();
  static void init_task(BTaskInfoBase* taskInfo, BTaskWrapper wrapper);

  template<typename T>
  static BTaskInfoBase* alloc_task(BTask<T> task, typename BTask<T>::ArgumentType arg, unsigned stackSize) {
    auto size = sizeof(BTaskInfo<T>) + stackSize + context_size();
    auto block = new uint8_t[size];
    auto taskInfo = new (block) BTaskInfo<T>();
    taskInfo->delegate = task;
    taskInfo->arg = arg;
    taskInfo->sp = &block[size - 1];
    return taskInfo;
  }

  template<typename T>
  static void task_wrapper(BTaskInfo<T>* taskInfo) {
    taskInfo->delegate(taskInfo->arg);
    kill_task(current_task_id());
    while (1)
      ;
  }

  template<typename T>
  static int run_task(BTask<T> task, typename BTask<T>::ArgumentType arg, uint8_t priority, unsigned stackSize) {
    if (priority > TaskPriority::Low) {
      return -1;
    }

    auto sreg = disable();
    auto new_task = 0;
    while (new_task < _tasks.Length() && _tasks[new_task]) {
      ++new_task;
    }

    auto taskInfo = alloc_task(task, arg, stackSize);
    if (new_task == _tasks.Length()) {
      _tasks.Add(taskInfo);
    } else {
      _tasks[new_task] = taskInfo;
    }

    taskInfo->id = new_task;
    taskInfo->priority = priority;
    ++_pri[priority].count;

    init_task(taskInfo, (BTaskWrapper)task_wrapper<T>);

    restore(sreg);
    return new_task;
  }
  template<typename T>
  friend int runTask(void (*task)(T arg), T arg, uint8_t priority, unsigned stackSize);
  friend void killTask(int id);
  friend void setupTasks(unsigned tasks);

  __BTASKSWITCHER_ARCH_CLASS__
};


template<typename T>
int runTask(void (*task)(T arg), T arg, uint8_t priority, unsigned stackSize) {
  BTaskSwitcher::run_task<T>(BTask<T>(task), arg, priority, stackSize);
}

void killTask(int id);
void setupTasks(unsigned tasks);

#endif