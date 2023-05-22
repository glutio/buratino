#ifdef ARDUINO_ARCH_AVR
#include <Arduino.h>
#include "BTaskSwitcher.h"

namespace B {

struct Ctx {
  uint8_t r31;
  uint8_t r30;
  uint8_t r29;
  uint8_t r28;
  uint8_t r27;
  uint8_t r26;
  uint8_t r25;
  uint8_t r24;
  uint8_t r23;
  uint8_t r22;
  uint8_t r21;
  uint8_t r20;
  uint8_t r19;
  uint8_t r18;
  uint8_t r17;
  uint8_t r16;
  uint8_t r15;
  uint8_t r14;
  uint8_t r13;
  uint8_t r12;
  uint8_t r11;
  uint8_t r10;
  uint8_t r9;
  uint8_t r8;
  uint8_t r7;
  uint8_t r6;
  uint8_t r5;
  uint8_t r4;
  uint8_t r3;
  uint8_t r2;
  uint8_t r1;
  uint8_t sreg;
  uint8_t r0;
};

bool disable() {
  auto sreg = SREG;
  noInterrupts();
  return sreg & _BV(SREG_I);
}

void __attribute__((naked)) switch_context(uint8_t** old_sp, uint8_t* new_sp) {
  asm volatile("push r0");
  asm volatile("in r0, __SREG__");
  asm volatile("push r0");
  asm volatile("push r1");
  asm volatile("push r2");
  asm volatile("push r3");
  asm volatile("push r4");
  asm volatile("push r5");
  asm volatile("push r6");
  asm volatile("push r7");
  asm volatile("push r8");
  asm volatile("push r9");
  asm volatile("push r10");
  asm volatile("push r11");
  asm volatile("push r12");
  asm volatile("push r13");
  asm volatile("push r14");
  asm volatile("push r15");
  asm volatile("push r16");
  asm volatile("push r17");
  asm volatile("push r18");
  asm volatile("push r19");
  asm volatile("push r20");
  asm volatile("push r21");
  asm volatile("push r22");
  asm volatile("push r23");
  asm volatile("push r24");
  asm volatile("push r25");
  asm volatile("push r26");
  asm volatile("push r27");
  asm volatile("push r28");
  asm volatile("push r29");
  asm volatile("push r30");
  asm volatile("push r31");
  asm volatile("movw r30, r24");
  asm volatile("in r0, __SP_L__");
  asm volatile("in r1, __SP_H__");
  asm volatile("std Z+0, r0");
  asm volatile("std Z+1, r1");

  asm volatile("out __SP_L__, r22");  // second argument
  asm volatile("out __SP_H__, r23");
  asm volatile("pop r31");
  asm volatile("pop r30");
  asm volatile("pop r29");
  asm volatile("pop r28");
  asm volatile("pop r27");
  asm volatile("pop r26");
  asm volatile("pop r25");
  asm volatile("pop r24");
  asm volatile("pop r23");
  asm volatile("pop r22");
  asm volatile("pop r21");
  asm volatile("pop r20");
  asm volatile("pop r19");
  asm volatile("pop r18");
  asm volatile("pop r17");
  asm volatile("pop r16");
  asm volatile("pop r15");
  asm volatile("pop r14");
  asm volatile("pop r13");
  asm volatile("pop r12");
  asm volatile("pop r11");
  asm volatile("pop r10");
  asm volatile("pop r9");
  asm volatile("pop r8");
  asm volatile("pop r7");
  asm volatile("pop r6");
  asm volatile("pop r5");
  asm volatile("pop r4");
  asm volatile("pop r3");
  asm volatile("pop r2");
  asm volatile("pop r1");
  asm volatile("pop r0");
  asm volatile("out __SREG__, r0");
  asm volatile("pop r0");
  asm volatile("ret");
}

void init_task(BTaskInfo* taskInfo) {
  // push task_wrapper address for `ret` to pop
  *taskInfo->sp-- = lowByte((uintptr_t)task_wrapper);
  *taskInfo->sp-- = highByte((uintptr_t)task_wrapper);
#if defined(__AVR_ATmega2560__)
  *taskInfo->sp-- = 0;  // for devices with more than 128kb program memory
#endif

  // clear registers
  for (unsigned i = 0; i < sizeof(Ctx); ++i) {
    *taskInfo->sp-- = 0;
  }

  Ctx* ctx = (Ctx*)(taskInfo->sp + 1);
  ctx->sreg = _BV(SREG_I);  // r25
  // compiler/architecture specific, passing argument via registers
  ctx->r24 = lowByte((uintptr_t)taskInfo);   // r24
  ctx->r25 = highByte((uintptr_t)taskInfo);  // r25
}

void switch_task() {
  auto next_task = queue_task();
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

  switch_context(&taskInfo->sp, _tasks[next_task]->sp);

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

void init_arch() {
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
}
#endif