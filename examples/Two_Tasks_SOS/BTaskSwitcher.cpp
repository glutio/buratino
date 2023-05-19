#include <new>
#include <Arduino.h>
#include "BList.h"
#include "BTaskSwitcher.h"

namespace B {

int8_t context_size();
void __attribute__((naked)) switch_context(uint8_t**, uint8_t*);
int init_task(BTaskInfo*);
void init_arch();
bool disable();

void restore(bool enable) {
  if (enable) interrupts();
  else noInterrupts();
}

BList<BTaskInfo*> _tasks;
int current_task = 0;

BTaskInfo* alloc_task(uintptr_t stackSize) {
  auto size = (stackSize - 1) + context_size();
  auto block = new uint8_t[sizeof(BTaskInfo) + size];
  auto taskInfo = new (block) BTaskInfo();
  taskInfo->sp = &taskInfo->stack[size - 1];
  return taskInfo;
}

void free_task(int id) {
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

void switch_task() {
  // switch_task assumes interrupts are disabled
  if (!_tasks.Length()) {
    return;
  }

  auto next_task = current_task;
  do {
    next_task = (next_task + 1) % _tasks.Length();
  } while (!_tasks[next_task] || _tasks[next_task]->id < 0);

  if (next_task == current_task) {
    return;
  }

  auto old_task = current_task;
  current_task = next_task;

  auto taskInfo = _tasks[old_task];
  // if current task is killed free its memory
  if (taskInfo->id < 0) {
    free_task(old_task);
  }
  SerialUSB.printf("O: %u %u\n", old_task, (uintptr_t)_tasks[old_task]->sp);
  SerialUSB.printf("N: %u %u\n", next_task, (uintptr_t)_tasks[next_task]->sp);
  // SerialUSB.printf(_tasks[next_task] == _tasks[old_task] ? "Y\n" : "N\n");
  //*(uint32_t*)_tasks[next_task]->sp = 0;
  //SerialUSB.println((uintptr_t)_tasks[next_task]->sp);
  switch_context(&taskInfo->sp, _tasks[next_task]->sp);
  SerialUSB.printf("*O: %u %u\n", old_task, (uintptr_t)_tasks[old_task]->sp);
  SerialUSB.printf("*N: %u %u\n", next_task, (uintptr_t)_tasks[next_task]->sp);

  // current task switches back here
}

void kill_task(int id) {
  auto sreg = disable();
  if (id > 0 && id < _tasks.Length() && _tasks[id]) {
    if (id != current_task) {
      free_task(id);
    } else {
      _tasks[id]->id = -1;
      switch_task();
      // should not reach here
    }
  }
  restore(sreg);
}

void yield_task() {
  auto sreg = disable();
  switch_task();
  restore(sreg);
}

// used by arduino's delay()
void yield() {
  yield_task();
}

void task_wrapper(BTaskInfo* taskInfo) {
  SerialUSB.printf("wrapper: %u\n", taskInfo);
  //SerialUSB.printf("wrapper %u\n", taskInfo->arg);
  taskInfo->delegate(0, taskInfo->arg);
  SerialUSB.println("wrapper done");
  kill_task(current_task_id());
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

void initialize(int tasks) {
  _tasks.Resize(tasks + 1);  // 1 for main loop()

  // add the initial loop() task
  _tasks.Add(alloc_task(1));  // loop() already has a stack
  _tasks[0]->id = 0;

  init_arch();
}

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