#include <avr/io.h>
#include <avr/interrupt.h>
#include <new>
#include <Arduino.h>
#include "BList.h"
#include "BTaskSwitcher.h"

namespace B {

enum Ctx {
  r0,
  sreg,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  r16,
  r17,
  r18,
  r19,
  r20,
  r21,
  r22,
  r23,
  r24,
  r25,
  r26,
  r27,
  r28,
  r29,
  r30,
  r31,
  size
};

struct TaskInfoBase {
  int8_t id;
  BTask::ArgumentType* arg;
  BTask delegate;
};

struct TaskInfo : TaskInfoBase {
  uint8_t* sp;
  uint8_t stack[1];
};

BList<TaskInfo*> _tasks;
int8_t current_task = 0;

uint8_t disable() {
  auto x = SREG;
  noInterrupts();
  return x;
}

void restore(uint8_t sreg) {
  SREG = sreg;
}

TaskInfo* alloc_task(uint16_t stackSize) {
  auto block = new uint8_t[sizeof(TaskInfo) + (stackSize - 1)];
  auto taskInfo = new (block) TaskInfo();
  return taskInfo;
}

void free_task(int8_t id) {
  _tasks[id]->~TaskInfo();
  delete[](uint8_t*) _tasks[id];
  _tasks[id] = 0;
}

int8_t current_task_id() {
  auto sreg = disable();
  auto id = current_task;
  restore(sreg);
  return id;
}

#define SAVE_CONTEXT(rlow, rhigh) \
  asm volatile("push r0"); \
  asm volatile("in r0, __SREG__"); \
  asm volatile("push r0"); \
  asm volatile("push r1"); \ 
  asm volatile("push r2"); \
  asm volatile("push r3"); \
  asm volatile("push r4"); \
  asm volatile("push r5"); \
  asm volatile("push r6"); \
  asm volatile("push r7"); \
  asm volatile("push r8"); \
  asm volatile("push r9"); \
  asm volatile("push r10"); \
  asm volatile("push r11"); \
  asm volatile("push r12"); \
  asm volatile("push r13"); \
  asm volatile("push r14"); \
  asm volatile("push r15"); \
  asm volatile("push r16"); \
  asm volatile("push r17"); \
  asm volatile("push r18"); \
  asm volatile("push r19"); \
  asm volatile("push r20"); \
  asm volatile("push r21"); \
  asm volatile("push r22"); \
  asm volatile("push r23"); \
  asm volatile("push r24"); \
  asm volatile("push r25"); \
  asm volatile("push r26"); \
  asm volatile("push r27"); \
  asm volatile("push r28"); \
  asm volatile("push r29"); \
  asm volatile("push r30"); \
  asm volatile("push r31"); \
  asm volatile("movw r30, "#rlow); \
  asm volatile("in r0, __SP_L__"); \
  asm volatile("in r1, __SP_H__"); \
  asm volatile("std Z+0, r0"); \
  asm volatile("std Z+1, r1")

#define LOAD_CONTEXT(rlow, rhigh) \
  asm volatile("out __SP_L__, " #rlow); \
  asm volatile("out __SP_H__, " #rhigh); \
  asm volatile("pop r31"); \
  asm volatile("pop r30"); \
  asm volatile("pop r29"); \
  asm volatile("pop r28"); \
  asm volatile("pop r27"); \
  asm volatile("pop r26"); \
  asm volatile("pop r25"); \
  asm volatile("pop r24"); \
  asm volatile("pop r23"); \
  asm volatile("pop r22"); \
  asm volatile("pop r21"); \
  asm volatile("pop r20"); \
  asm volatile("pop r19"); \
  asm volatile("pop r18"); \
  asm volatile("pop r17"); \
  asm volatile("pop r16"); \
  asm volatile("pop r15"); \
  asm volatile("pop r14"); \
  asm volatile("pop r13"); \
  asm volatile("pop r12"); \
  asm volatile("pop r11"); \
  asm volatile("pop r10"); \
  asm volatile("pop r9"); \
  asm volatile("pop r8"); \
  asm volatile("pop r7"); \
  asm volatile("pop r6"); \
  asm volatile("pop r5"); \
  asm volatile("pop r4"); \
  asm volatile("pop r3"); \
  asm volatile("pop r2"); \
  asm volatile("pop r1"); \
  asm volatile("pop r0"); \
  asm volatile("out __SREG__, r0"); \
  asm volatile("pop r0"); \
  asm volatile("ret");

void __attribute__((naked)) switch_context(uint8_t** old_sp, uint8_t* new_sp) {
  SAVE_CONTEXT(r24, r25);
  LOAD_CONTEXT(r22, r23);  // use register r24,25 for first argument (compiler specific)
}

void __attribute__((naked)) restore_context(uint8_t* sp) {
  LOAD_CONTEXT(r24, r25);  // registers used to pass first parameter (compiler specific)
}

void switch_task() {
  // switch_task assumes interrupts are disabled
  auto next_task = current_task;
  do {
    next_task = (next_task + 1) % _tasks.Length();
  } while (!_tasks[next_task] || _tasks[next_task]->id < 0);

  if (next_task == current_task) {
    return;
  }

  auto old_task = current_task;
  current_task = next_task;

  // if current task is killed free its memory
  if (_tasks[old_task]->id < 0) {
    free_task(old_task);
    restore_context(_tasks[next_task]->sp);
    // never returns
  }

  switch_context(&_tasks[old_task]->sp, _tasks[next_task]->sp);
  // current task switches back here
}

void kill_task(int8_t id) {
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

void task_wrapper(TaskInfo* taskInfo) {
  taskInfo->delegate(0, taskInfo->arg);
  kill_task(current_task_id());
}

int8_t run_task(BTask& task, BTask::ArgumentType* arg, uint16_t stackSize) {
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
  taskInfo->sp = &taskInfo->stack[stackSize - 1];
  *taskInfo->sp-- = 0xAA;

  // push task_wrapper address for `ret` to pop
  *taskInfo->sp-- = lowByte((uintptr_t)task_wrapper);
  *taskInfo->sp-- = highByte((uintptr_t)task_wrapper);
#if defined(__AVR_ATmega2560__)
  *taskInfo->sp-- = 0;  // for devices with more than 128kb program memory
#endif

  *taskInfo->sp-- = 0;     // r0
  *taskInfo->sp-- = 0x80;  // SREG, enable interrupts

  // clear registers
  for (uint8_t i = Ctx::r1; i < Ctx::size; ++i) {
    *taskInfo->sp-- = 0;
  }

  // compiler/architecture specific, passing argument via registers
  *(taskInfo->sp + Ctx::size - Ctx::r24) = lowByte((uintptr_t)taskInfo);   // r24
  *(taskInfo->sp + Ctx::size - Ctx::r25) = highByte((uintptr_t)taskInfo);  // r25

  restore(sreg);
  return new_task;
}

void initialize(int8_t tasks) {
  _tasks.Resize(tasks + 1);  // 1 for main loop()

  // add the initial loop() task
  _tasks.Add(alloc_task(1));  // loop() already has a stack
  _tasks[0]->id = 0;
}

void setup_timer() {
  auto sreg = disable();

  TCCR1A = 0;  // set timer for normal operation
  TCCR1B = 0;  // clear register
  TCNT1 = 0;   // zero timer

  OCR1A = 16000 - 1;        // load compare register: 16MHz/1/1000Hz = 16000 - 1 (for 1ms tick)
  TCCR1B |= (1 << WGM12);   // CTC mode, no prescaler: CS12 = 0 and CS10 = 1
  TCCR1B |= (1 << CS10);    // set prescaler to 1
  TIMSK1 |= (1 << OCIE1A);  // enable compare match interrupt

  restore(sreg);
}

}

ISR(TIMER1_COMPA_vect) {
  B::switch_task();
}

BTaskSwitcher::BTaskSwitcher() {
}

void BTaskSwitcher::Setup(int8_t tasks) {
  B::initialize(tasks);
}

void BTaskSwitcher::Start() {
  B::setup_timer();
}

int8_t BTaskSwitcher::RunTask(BTask& delegate, BTask::ArgumentType* arg, uint16_t stackSize) {
  B::run_task(delegate, arg, stackSize);
}

void BTaskSwitcher::KillTask(int8_t id) {
  B::kill_task(id);
}

void BTaskSwitcher::YieldTask() {
  B::yield_task();
}

int8_t BTaskSwitcher::CurrentTask() {
  return B::current_task_id();
}