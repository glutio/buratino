#include <avr/io.h>
#include <avr/interrupt.h>
#include <new>
#include <Arduino.h>
#include "BList.h"
#include "BTaskSwitcher.h"

enum Ctx {
  spl = 32,
  sph,
  sreg,
  size
};

struct TaskInfoBase {
  int8_t id;
  BTask::ArgumentType* arg;
  BTask delegate;
};

struct TaskInfo : TaskInfoBase {
  uint8_t ctx[Ctx::size];
  uint8_t stack[1];
};

BList<TaskInfo*> _tasks;
int8_t current_task = 0;

uint8_t disable() {
  auto x = SREG;
  noInterrupts();
  return x;
}

void restore(uint8_t x) {
  SREG = x;
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

// original context switch work (C) by Michael Minor
// https://github.com/9MMMinor/avrXinu-V7/blob/master/avr-Xinu/src/sys/sys/ctxsw.S
#define SaveContext(r) \
  asm volatile("push r31"); \
  asm volatile("push r30"); \
  asm volatile("movw r30, " #r); \
  asm volatile("std Z+0, r0"); \
  asm volatile("in r0, __SREG__"); \
  asm volatile("std Z+34, r0"); \
  asm volatile("std Z+1, r1"); \
  asm volatile("std Z+2, r2"); \
  asm volatile("std Z+3, r3"); \
  asm volatile("std Z+4, r4"); \
  asm volatile("std Z+5, r5"); \
  asm volatile("std Z+6, r6"); \
  asm volatile("std Z+7, r7"); \
  asm volatile("std Z+8, r8"); \
  asm volatile("std Z+9, r9"); \
  asm volatile("std Z+10, r10"); \
  asm volatile("std Z+11, r11"); \
  asm volatile("std Z+12, r12"); \
  asm volatile("std Z+13, r13"); \
  asm volatile("std Z+14, r14"); \
  asm volatile("std Z+15, r15"); \
  asm volatile("std Z+16, r16"); \
  asm volatile("std Z+17, r17"); \
  asm volatile("std Z+18, r18"); \
  asm volatile("std Z+19, r19"); \
  asm volatile("std Z+20, r20"); \
  asm volatile("std Z+21, r21"); \
  asm volatile("std Z+22, r22"); \
  asm volatile("std Z+23, r23"); \
  asm volatile("std Z+24, r24"); \
  asm volatile("std Z+25, r25"); \
  asm volatile("std Z+26, r26"); \
  asm volatile("std Z+27, r27"); \
  asm volatile("std Z+28, r28"); \
  asm volatile("std Z+29, r29"); \
  asm volatile("pop r0"); \
  asm volatile("std Z+30, r0"); \
  asm volatile("pop r0"); \
  asm volatile("std Z+31, r0"); \
  asm volatile("in r0, __SP_L__"); \
  asm volatile("std Z+32, r0"); \
  asm volatile("in r0, __SP_H__"); \
  asm volatile("std Z+33, r0");

#define LoadContext(r) \
  asm volatile("movw r30, " #r); \
  asm volatile("ldd r0, Z+32"); \
  asm volatile("out __SP_L__, r0"); \
  asm volatile("ldd r0, Z+33"); \
  asm volatile("out __SP_H__, r0"); \
  asm volatile("ldd r0, Z+31"); \
  asm volatile("push r0"); \
  asm volatile("ldd r0, Z+30"); \
  asm volatile("push r0"); \
  asm volatile("ldd r29, Z+29"); \
  asm volatile("ldd r28, Z+28"); \
  asm volatile("ldd r27, Z+27"); \
  asm volatile("ldd r26, Z+26"); \
  asm volatile("ldd r25, Z+25"); \
  asm volatile("ldd r24, Z+24"); \
  asm volatile("ldd r23, Z+23"); \
  asm volatile("ldd r22, Z+22"); \
  asm volatile("ldd r21, Z+21"); \
  asm volatile("ldd r20, Z+20"); \
  asm volatile("ldd r19, Z+19"); \
  asm volatile("ldd r18, Z+18"); \
  asm volatile("ldd r17, Z+17"); \
  asm volatile("ldd r16, Z+16"); \
  asm volatile("ldd r15, Z+15"); \
  asm volatile("ldd r14, Z+14"); \
  asm volatile("ldd r13, Z+13"); \
  asm volatile("ldd r12, Z+12"); \
  asm volatile("ldd r11, Z+11"); \
  asm volatile("ldd r10, Z+10"); \
  asm volatile("ldd r9, Z+9"); \
  asm volatile("ldd r8, Z+8"); \
  asm volatile("ldd r7, Z+7"); \
  asm volatile("ldd r6, Z+6"); \
  asm volatile("ldd r5, Z+5"); \
  asm volatile("ldd r4, Z+4"); \
  asm volatile("ldd r3, Z+3"); \
  asm volatile("ldd r2, Z+2"); \
  asm volatile("ldd r1, Z+1"); \
  asm volatile("ldd r0, Z+34"); \
  asm volatile("out __SREG__, r0"); \
  asm volatile("ldd r0, Z+0"); \
  asm volatile("pop r30"); \
  asm volatile("pop r31"); \
  asm volatile("ret");

void __attribute__((naked)) switch_context(uint8_t* oldctx, uint8_t* newctx) {
  SaveContext(r24); // use register r24 for first argument (compiler specific)
  LoadContext(r22); // use r22 for second argument (newctx)
}

void __attribute__((naked)) restore_context(uint8_t* ctx) {
  LoadContext(r24);
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
    restore_context(_tasks[next_task]->ctx);
    // never returns
  }

  switch_context(_tasks[old_task]->ctx, _tasks[next_task]->ctx);
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

  // clear registers
  for (auto i = 0; i < Ctx::size; ++i) {
    taskInfo->ctx[i] = 0;
  }

  // compiler/architecture specific, passing argument via registers
  taskInfo->ctx[24] = lowByte((uintptr_t)taskInfo);   // r24
  taskInfo->ctx[25] = highByte((uintptr_t)taskInfo);  // r25

  uint8_t* sp = &taskInfo->stack[stackSize - 1];
  *sp-- = 0xAA;  // magic number indicating bottom of stack

  // push task_wrapper address for `ret` to pop
  *sp-- = lowByte((uintptr_t)task_wrapper);
  *sp-- = highByte((uintptr_t)task_wrapper);
#if defined(__AVR_ATmega2560__)
  *sp-- = 0;  // for devices with more than 128kb program memory
#endif

  // save the stack in the context
  taskInfo->ctx[Ctx::spl] = lowByte((uintptr_t)sp);
  taskInfo->ctx[Ctx::sph] = highByte((uintptr_t)sp);

  taskInfo->ctx[Ctx::sreg] = 0x80;  // SREG

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

ISR(TIMER1_COMPA_vect) {
  switch_task();
}

BTaskSwitcher::BTaskSwitcher() {
}

void BTaskSwitcher::Setup(int8_t tasks) {
  initialize(tasks);
}

void BTaskSwitcher::Start() {
  setup_timer();
}

int8_t BTaskSwitcher::RunTask(BTask& delegate, BTask::ArgumentType* arg, uint16_t stackSize) {
  run_task(delegate, arg, stackSize);
}

void BTaskSwitcher::KillTask(int8_t id) {
  kill_task(id);
}

void BTaskSwitcher::YieldTask() {
  yield_task();
}

int8_t BTaskSwitcher::CurrentTask() {
  return current_task_id();
}