#include <Arduino.h>
#include "BList.h"
#include "BTask.h"
#include "BTaskSwitcher.h"

volatile bool BTaskSwitcher::_initialized = false;
BList<BTaskSwitcher::BTaskInfoBase*> BTaskSwitcher::_tasks;
volatile unsigned BTaskSwitcher::_current_task = 0;
volatile unsigned BTaskSwitcher::_next_task = 0;
volatile unsigned BTaskSwitcher::_yielded_task = -1;

BTaskSwitcher::BSwitchState BTaskSwitcher::_pri[3] = { 0 };

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
  _tasks[id]->~BTaskInfoBase();
  delete[](uint8_t*) _tasks[id];
  _tasks[id] = 0;
}

void BTaskSwitcher::kill_task(int id) {
  auto sreg = disable();
  if (id > 0 && id < _tasks.Length() && _tasks[id]) {
    --_pri[_tasks[id]->priority].count;

    if (id == _current_task) {
      _tasks[id]->id = -1;
      yield_task();
      restore(sreg);
      while (1)
        ;
    }

    free_task(id);
  }
  restore(sreg);
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

  if (!_pri[pri].count || (_pri[pri].count == 1 && _pri[pri].current == _yielded_task)) {
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
    if (i >= 0 && i < len) {  // found another priority level
      pri = i;
    } else  // no other tasks found
    {
      return _yielded_task;
    }
  }

  auto next_task = _pri[pri].current;
  do {
    ++next_task;
    if (next_task >= _tasks.Length()) {
      next_task = 0;
    }
  } while (next_task != _pri[pri].current && (next_task == _yielded_task || !_tasks[next_task] || _tasks[next_task]->id < 0 || _tasks[next_task]->priority != pri));
  _pri[pri].current = next_task;

  return next_task;
}

uint8_t* BTaskSwitcher::swap_stack(uint8_t* sp) {
  auto sreg = disable();
  if (_tasks[_current_task]->id < 0) {
    free_task(_current_task);
  } else {
    _tasks[_current_task]->sp = sp;
  }

  _current_task = _next_task;
  _yielded_task = -1;

  sp = _tasks[_current_task]->sp;
  restore(sreg);
  return sp;
}

void BTaskSwitcher::schedule_task() {
  if (_initialized && _current_task == _next_task) {
    _next_task = get_next_task();
    if (_next_task != _current_task) {
      switch_context();
    }
  }
}

void BTaskSwitcher::yield_task() {
  auto sreg = disable();
  _yielded_task = _current_task;
  schedule_task();
  _yielded_task = -1;
  restore(sreg);
  return;
}

void BTaskSwitcher::initialize(unsigned tasks) {
  if (!_initialized) {
    _tasks.Resize(tasks + 1);  // 1 for main loop()

    // add the initial loop() task
    _tasks.Add(new BTaskInfoBase());  // loop() already has a stack
    _tasks[0]->id = 0;
    _tasks[0]->priority = TaskPriority::Medium;
    _pri[_tasks[0]->priority].count = 1;

    init_arch();

    _initialized = true;
  }
}

void killTask(int id) {
  BTaskSwitcher::kill_task(id);
}

void setupTasks(unsigned tasks) {
  BTaskSwitcher::initialize(tasks);
}

// copy of standard delay function wrapping call to micros()
// call to micros() must be synchronized for task switching to work
void delayTask(unsigned long ms) {
  if (ms == 0) {
    return;
  }

  auto sreg = BTaskSwitcher::disable();
  uint32_t start = micros();
  BTaskSwitcher::restore(sreg);
  while (ms > 0) {
    yield();
    while (ms > 0) {
      sreg = BTaskSwitcher::disable();
      auto _micros = micros();
      BTaskSwitcher::restore(sreg);
      if ((_micros - start) < 1000) {
        break;
      }
      ms--;
      start += 1000;
    }
  }
}

//used by arduino's delay()
void yield() {
  BTaskSwitcher::yield_task();
}