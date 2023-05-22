#include <new>
#include <Arduino.h>
#include "BList.h"
#include "BTaskSwitcher.h"

namespace B {
static BList<BTaskInfo*> _tasks;
static int current_task = 0;

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
  _tasks[id]->~BTaskInfo();
  delete[](uint8_t*) _tasks[id];
  _tasks[id] = 0;
}

int current_task_id() {
  auto sreg = disable();
  auto id = current_task;
  restore(sreg);
  return id;
}

static int queue_task() {
  auto next_task = current_task;
  do {
    next_task = (next_task + 1) % _tasks.Length();
  } while (!_tasks[next_task] || _tasks[next_task]->id < 0);

  return next_task;
}

static void task_wrapper(BTaskInfo* taskInfo) {
  taskInfo->delegate(0, taskInfo->arg);
  kill_task(current_task_id());
}

static BTaskInfo* alloc_task(uintptr_t stackSize) {
  auto size = (stackSize - 1) + sizeof(Ctx);
  auto block = new uint8_t[sizeof(BTaskInfo) + size];
  auto taskInfo = new (block) BTaskInfo();
  taskInfo->sp = &taskInfo->stack[size - 1];
  return taskInfo;
}

int run_task(BTask& task, BTask::ArgumentType* arg, uintptr_t stackSize) {
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

  init_task(taskInfo);
  restore(sreg);
  return new_task;
}

static void initialize(int tasks) {
  _tasks.Resize(tasks + 1);  // 1 for main loop()

  // add the initial loop() task
  _tasks.Add(alloc_task(1));  // loop() already has a stack
  _tasks[0]->id = 0;

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

int BTaskSwitcher::RunTask(BTask& delegate, BTask::ArgumentType* arg, uintptr_t stackSize) {
  B::run_task(delegate, arg, stackSize);
}

void BTaskSwitcher::KillTask(int id) {
  B::kill_task(id);
}

void BTaskSwitcher::YieldTask() {
  B::yield_task();
}

int BTaskSwitcher::CurrentTask() {
  return B::current_task_id();
}