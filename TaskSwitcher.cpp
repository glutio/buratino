#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "Buratino.h"
#include "TaskSwitcher.h"

enum Ctx
{
  spl = 32,
  sph,
  sreg,
  size
};

struct TaskInfo : public TaskInfoBase {
  uint8_t* stack;
  uint8_t* sp;
  uint8_t* ctx;
  TaskInfo()
    : stack(0), sp(0), ctx(0) {}
};

List<TaskInfo*> _tasks(10);
int8_t current_task = 0;

int8_t get_task_id() {
  return current_task;
}

// original work (C) by Michael Minor
// https :  //github.com/9MMMinor/avrXinu-V7/blob/master/avr-Xinu/src/sys/sys/ctxsw.S
void* __attribute__((naked)) switch_context(uint8_t* oldctx, uint8_t* newctx) {
  // load oldctx to r31:r32
  asm volatile("push r31");
  asm volatile("push r30");
  asm volatile("movw r30, r24");

  // save registers to oldctx
  asm volatile("std Z+0, r0");
  asm volatile("in r0, __SREG__");
  asm volatile("cli");
  asm volatile("std Z+34, r0");
  asm volatile("std Z+1, r1");
  asm volatile("std Z+2, r2");
  asm volatile("std Z+3, r3");
  asm volatile("std Z+4, r4");
  asm volatile("std Z+5, r5");
  asm volatile("std Z+6, r6");
  asm volatile("std Z+7, r7");
  asm volatile("std Z+8, r8");
  asm volatile("std Z+9, r9");
  asm volatile("std Z+10, r10");
  asm volatile("std Z+11, r11");
  asm volatile("std Z+12, r12");
  asm volatile("std Z+13, r13");
  asm volatile("std Z+14, r14");
  asm volatile("std Z+15, r15");
  asm volatile("std Z+16, r16");
  asm volatile("std Z+17, r17");
  asm volatile("std Z+18, r18");
  asm volatile("std Z+19, r19");
  asm volatile("std Z+20, r20");
  asm volatile("std Z+21, r21");
  asm volatile("std Z+22, r22");
  asm volatile("std Z+23, r23");
  asm volatile("std Z+24, r24");
  asm volatile("std Z+25, r25");
  asm volatile("std Z+26, r26");
  asm volatile("std Z+27, r27");
  asm volatile("std Z+28, r28");
  asm volatile("std Z+29, r29");
  asm volatile("pop r0");
  asm volatile("std Z+30, r0");
  asm volatile("pop r0");
  asm volatile("std Z+31, r0");
  asm volatile("in r0, __SP_L__");
  asm volatile("std Z+32, r0");
  asm volatile("in r0, __SP_H__");
  asm volatile("std Z+33, r0");

  // switch argument pointer
  asm volatile("movw r30, r22");

  // switch stacks
  asm volatile("ldd r0, Z+32");
  asm volatile("out __SP_L__, r0");
  asm volatile("ldd r0, Z+33");
  asm volatile("out __SP_H__, r0");

  // restore registers
  asm volatile("ldd r0, Z+31");
  asm volatile("push r0");
  asm volatile("ldd r0, Z+30");
  asm volatile("push r0");
  asm volatile("ldd r29, Z+29");
  asm volatile("ldd r28, Z+28");
  asm volatile("ldd r27, Z+27");
  asm volatile("ldd r26, Z+26");
  asm volatile("ldd r25, Z+25");
  asm volatile("ldd r24, Z+24");
  asm volatile("ldd r23, Z+23");
  asm volatile("ldd r22, Z+22");
  asm volatile("ldd r21, Z+21");
  asm volatile("ldd r20, Z+20");
  asm volatile("ldd r19, Z+19");
  asm volatile("ldd r18, Z+18");
  asm volatile("ldd r17, Z+17");
  asm volatile("ldd r16, Z+16");
  asm volatile("ldd r15, Z+15");
  asm volatile("ldd r14, Z+14");
  asm volatile("ldd r13, Z+13");
  asm volatile("ldd r12, Z+12");
  asm volatile("ldd r11, Z+11");
  asm volatile("ldd r10, Z+10");
  asm volatile("ldd r9, Z+9");
  asm volatile("ldd r8, Z+8");
  asm volatile("ldd r7, Z+7");
  asm volatile("ldd r6, Z+6");
  asm volatile("ldd r5, Z+5");
  asm volatile("ldd r4, Z+4");
  asm volatile("ldd r3, Z+3");
  asm volatile("ldd r2, Z+2");
  asm volatile("ldd r1, Z+1");
  asm volatile("ldd r0, Z+34");      // SREG
  asm volatile("out __SREG__, r0");  // may enable interrupts
  asm volatile("ldd r0, Z+0");
  asm volatile("pop r30");
  asm volatile("pop r31");

  asm volatile("ret");
}
void task_wrapper(TaskInfo*);
void switch_task() {
  auto next_task = current_task;
  do {
    next_task = (next_task + 1) % _tasks.Length();
  } while (_tasks[next_task]->id < 0);

  if (next_task == current_task) {
    return;
  }

  auto old_task = current_task;
  current_task = next_task;

  Serial.print(old_task);
  Serial.print(" ");
  Serial.println(next_task);
  for (auto i = 0; i < Ctx::size; i++) {
    Serial.print(_tasks[next_task]->ctx[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  for (auto i = 511; i > 511 - 16; i--) {
    Serial.print(_tasks[next_task]->stack[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println((uintptr_t)task_wrapper, HEX);
  uint8_t* sp = (uint16_t)_tasks[next_task]->ctx[33] << 8 | _tasks[next_task]->ctx[32];
  uintptr_t x = (uint16_t) * (sp + 1) << 8 | *(sp + 2);
  Serial.println(x, HEX);
  Serial.flush();
  switch_context(_tasks[old_task]->ctx, _tasks[next_task]->ctx);
  Serial.print("back ");
  Serial.println(current_task);
  Serial.flush();
}

void kill_task(uint8_t id) {
  noInterrupts();

  _tasks[id]->id = -1;
  delete[] _tasks[id]->stack;
  _tasks[id]->stack = 0;

  switch_task();
  // should not reach here
}

void task_wrapper(TaskInfo* taskInfo) {
  // get TaskInfo pointer from the stack
  digitalWrite(6, HIGH);

  taskInfo->delegate(&Buratino::_instance, taskInfo->arg);
  Serial.println("task wrapper");
  Serial.flush();
  kill_task(get_task_id());
}

void run_task(BTask& task, BTask::Argument* arg, int16_t stackSize) {
  noInterrupts();
  auto new_task = 0;

  while (_tasks[new_task]->id >= 0 && new_task < _tasks.Length()) ++new_task;

  if (new_task == _tasks.Length()) {
    _tasks.Add(new TaskInfo());
  };

  Serial.print(new_task);
  Serial.print("_");
  Serial.println(_tasks.Length());
  Serial.flush();

  auto taskInfo = _tasks[new_task];

  taskInfo->id = new_task;
  taskInfo->delegate = task;
  taskInfo->arg = arg;
  taskInfo->stack = new uint8_t[stackSize];

  // clear stack
  for (auto i = 0; i < stackSize; ++i) {
    taskInfo->stack[i] = 0;
  }

  if (!taskInfo->ctx) {
    taskInfo->ctx = new uint8_t[Ctx::size];
  }

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
  *sp-- = 0;

  // save the stack in the context
  taskInfo->ctx[Ctx::spl] = lowByte((uintptr_t)sp);
  taskInfo->ctx[Ctx::sph] = highByte((uintptr_t)sp);

  taskInfo->ctx[Ctx::sreg] = 0x80;  // SREG
  taskInfo->sp = sp;

  interrupts();
}

void initialize() {
  // add the initial loop() task
  _tasks.Add(new TaskInfo());
  _tasks[0]->id = 0;
  _tasks[0]->ctx = new uint8_t[Ctx::size];
}

void cleanup_tasks() {
  noInterrupts();
  for (auto i = 0; i < _tasks.Length(); ++i) {
    if (_tasks[i]->id < 0 && _tasks[i]->ctx) {
      delete[] _tasks[i]->ctx;
      _tasks[i]->ctx = 0;
    }
  }
  interrupts();
}

void setup_timer() {
  noInterrupts();

  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B

  // set compare match register to desired timer count:
  OCR1A = 15624;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);


  // // Set CTC mode
  // TCCR1A = 0;
  // TCCR1B = (1 << WGM12);

  // // Set prescaler to 64
  // TCCR1B |= (1 << CS11) | (1 << CS10);

  // // Set Output Compare Match Count for 1ms
  // OCR1A = 250;

  // // Enable Output Compare Match Interrupt
  // TIMSK1 = (1 << OCIE1A);



  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  noInterrupts();
  switch_task();
  interrupts();
}

TaskSwitcher::TaskSwitcher() {
  initialize();
}

void TaskSwitcher::YieldTask() {
  noInterrupts();
  switch_task();
  interrupts(); 
}

void TaskSwitcher::Setup() {
  setup_timer();
}

void TaskSwitcher::RunTask(BTask delegate, BTask::Argument* arg, uint16_t stackSize) {
  run_task(delegate, arg, stackSize);
}

void TaskSwitcher::Cleanup() {
  cleanup_tasks();
}

TaskInfoBase::TaskInfoBase()
  : id(-1), arg(0) {}
