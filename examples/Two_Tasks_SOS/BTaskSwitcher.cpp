#include <Arduino.h>
#include "BList.h"
#include "BTask.h"
#include "BTaskSwitcher.h"

BList<BTaskSwitcher::BTaskInfoBase*> BTaskSwitcher::_tasks;
unsigned BTaskSwitcher::_current_task = 0;
unsigned BTaskSwitcher::_next_task = 0;
BTaskSwitcher::BSwitchState BTaskSwitcher::_pri[3] = {0};

void BTaskSwitcher::restore(bool enable) {
  if (enable) interrupts();
  else noInterrupts();
}

int BTaskSwitcher::current_task_id() {
  auto sreg = disable();
  auto id = _current_task;
  restore(sreg);
  return id;
}

void BTaskSwitcher::free_task(int id) {
  --_pri[_tasks[id]->priority].count;
  _tasks[id]->~BTaskInfoBase();
  delete[](uint8_t*) _tasks[id];
  _tasks[id] = 0;
}

int BTaskSwitcher::get_next_task() {
  const int high = 50;        //62;
  const int med = high + 33;  //24
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

void BTaskSwitcher::initialize(unsigned tasks) {
  _tasks.Resize(tasks + 1);  // 1 for main loop()

  // add the initial loop() task
  _tasks.Add(alloc_task(BTask<void*>(), 0, 0));  // loop() already has a stack
  _tasks[0]->id = 0;
  _tasks[0]->priority = TaskPriority::Medium;
  _pri[_tasks[0]->priority].count = 1;
  init_arch();
}

void killTask(int id) {
  BTaskSwitcher::kill_task(id);
}

void setupTasks(unsigned tasks) {
  BTaskSwitcher::initialize(tasks);
}

//used by arduino's delay()
void yield() {
  // B::yield_task();
}