#include <new>
#include <Arduino.h>
#include "BList.h"
#include "BTaskSwitcher.h"

namespace B {
struct BTaskInfo {
  uint8_t* sp;  // needs to be aligned
  int id;
  uint8_t priority;
  BTask::ArgumentType* arg;
  BTask delegate;
  uint8_t stack[1];
};

struct SwitchState {
  int current;
  int count;
};

static BList<BTaskInfo*> _tasks;
static SwitchState _pri[3] = { 0 };
static int _current_task = 0;

static void restore(bool);
static void free_task(int);
static void task_wrapper(BTaskInfo*);
static int queue_task();
}

#include "TaskSwitcherAVR.h"
#include "TaskSwitcherSAMD.h"

namespace B {

static void restore(bool enable) {
  if (enable) interrupts();
  else noInterrupts();
}

static void free_task(int id) {
  --_pri[_tasks[id]->priority].count;
  _tasks[id]->~BTaskInfo();
  delete[](uint8_t*) _tasks[id];
  _tasks[id] = 0;
}

BTaskInfo& get_task(int id) {
  return *_tasks[id];
}

int _current_task_id() {
  auto sreg = disable();
  auto id = _current_task;
  restore(sreg);
  return id;
}

static int queue_task() {
  const int high = 62;
  const int med = high + 24;
  const int low = 100;

  auto dice = rand() % 100;

  int pri = 0;
  if (dice < high) pri = TaskPriority::High;
  else if (dice < med) pri = TaskPriority::Medium;
  else pri = TaskPriority::Low;

  if (!_pri[pri].count) {
    const auto len = sizeof(_pri) / sizeof(_pri[0]);
    int i;
    for (i = pri + 1; i < len; ++i) {
      if (_pri[i].count) break;
    }
    if (i == len) {
      for (i = pri - 1; i >= 0; --i) {
        if (_pri[i].count) break;
      }
    }
    pri = i;
  }

  auto& next_task = _pri[pri].current;
  do {
    next_task = (next_task + 1) % _tasks.Length();
  } while (!_tasks[next_task] || _tasks[next_task]->id < 0 || _tasks[next_task]->priority != pri);


  return next_task;
}

static void task_wrapper(BTaskInfo* taskInfo) {
  taskInfo->delegate(0, taskInfo->arg);
  kill_task(_current_task_id());
  while (1)
    ;
}

static BTaskInfo* alloc_task(uintptr_t stackSize) {
  auto size = (stackSize - 1) + sizeof(Ctx);
  auto block = new uint8_t[sizeof(BTaskInfo) + size];
  auto taskInfo = new (block) BTaskInfo();
  taskInfo->sp = &taskInfo->stack[size - 1];
  return taskInfo;
}

int run_task(BTask& task, BTask::ArgumentType* arg, uint8_t priority, uintptr_t stackSize) {
  if (priority > TaskPriority::Low) {
    return -1;
  }

  auto sreg = disable();
  auto new_task = 0;
  while (new_task < _tasks.Length() && _tasks[new_task]) {
    ++new_task;
  }

  auto taskInfo = alloc_task(stackSize);
  if (new_task == _tasks.Length()) {
    _tasks.Add(taskInfo);
  } else {
    _tasks[new_task] = taskInfo;
  }

  taskInfo->id = new_task;
  taskInfo->delegate = task;
  taskInfo->arg = arg;
  taskInfo->priority = priority;
  ++_pri[priority].count;

  init_task(taskInfo);
  restore(sreg);
  return new_task;
}

static void initialize(int tasks) {
  _tasks.Resize(tasks + 1);  // 1 for main loop()

  // add the initial loop() task
  _tasks.Add(alloc_task(1));  // loop() already has a stack
  _tasks[0]->id = 0;
  _tasks[0]->priority = TaskPriority::Medium;
  _pri[_tasks[0]->priority].count = 1;
  init_arch();
}

}

//used by arduino's delay()
void yield() {
  B::yield_task();
}

BTaskSwitcher::BTaskSwitcher() {
}

void BTaskSwitcher::Setup(int tasks) {
  B::initialize(tasks);
}

int BTaskSwitcher::RunTask(BTask& delegate, BTask::ArgumentType* arg, uint8_t priority, uintptr_t stackSize) {
  B::run_task(delegate, arg, priority, stackSize);
}

void BTaskSwitcher::KillTask(int id) {
  B::kill_task(id);
}

void BTaskSwitcher::YieldTask() {
  B::yield_task();
}

int BTaskSwitcher::CurrentTask() {
  return B::_current_task_id();
}