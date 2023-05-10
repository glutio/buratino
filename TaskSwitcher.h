#ifndef __TASKSWITCHER_H__
#define __TASKSWITCHER_H__

#include "Bind.h"

class TaskSwitcher;

template<typename T>
class List {
protected:
  T* _array;
  int16_t _count;
  int16_t _capacity;
public:
  List(int16_t capacity)
    : _capacity(capacity) {}

  T& operator[](int16_t index) {
    return _array[index];
  }

  Add(T& item) {
    if (_count == _capacity) {
      _capacity *= 2;
      T* array = new T[_capacity];
      for (auto i = 0; i < _capacity; ++i) {
        array[i] = _array[i];
      }
      delete[] _array;
      _array = array;
    }

    _array[_count++] = item;
  }

  int16_t Length() {
    return _count;
  }
};

class Task {
public:
  typedef EventDelegate<TaskSwitcher, void> TaskDelegate;
};

struct TaskInfo {
  uint8_t* stack;
  int8_t id;
  void* pc;
  uint8_t* stack_pointer;
  Task::TaskDelegate::Argument* arg;
  Task::TaskDelegate delegate;

  TaskInfo() : stack(0), id(-1), stack_pointer(0), arg(0) {}
};

class TaskSwitcher {
protected:
  List<TaskInfo> _tasks;
  int8_t current_task;
  static TaskSwitcher _instance;
public:  
  TaskSwitcher(Timer& timer)
    : _tasks(10), current_task(0){
        timer.OnTick = Timer::TickEvent(this, &TaskSwitcher::Switch);
      };

  void Switch(Timer* timer, TimerArgs* args) {
    auto new_task = current_task;
    do {
      new_task = (new_task + 1) % _tasks.Length();
    } while (!_tasks[new_task].stack_pointer);

    if (new_task != current_task) {
      SaveContext();
      current_task = new_task;
      RestoreContext();
    }
  }

  void AddTask(Task::TaskDelegate& task, Task::TaskDelegate::Argument* arg, int16_t stackSize);
  void CompleteTask(TaskInfo* taskInfo);
  void RunTask(TaskInfo* taskInfo);
  void DeleteTask(TaskInfo* taskInfo);
  
  virtual void SaveContext();
  virtual void RestoreContext();
};

#endif