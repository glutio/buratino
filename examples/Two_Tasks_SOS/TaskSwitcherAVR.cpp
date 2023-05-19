#ifdef ARDUINO_ARCH_AVR
#include <Arduino.h>
#include "BTaskSwitcher.h"

namespace B {

void switch_task();
void task_wrapper(BTaskInfo*);
void restore(bool);

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

int8_t __attribute__((inline)) context_size() {
  return Ctx::size;
}

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

  asm volatile("out __SP_L__, r22");
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
  for (unsigned i = 0; i < Ctx::size; ++i) {
    *taskInfo->sp-- = 0;
  }

  *(taskInfo->sp + Ctx::size - Ctx::sreg) = _BV(SREG_I);  // r25
  // compiler/architecture specific, passing argument via registers
  *(taskInfo->sp + Ctx::size - Ctx::r24) = lowByte((uintptr_t)taskInfo);   // r24
  *(taskInfo->sp + Ctx::size - Ctx::r25) = highByte((uintptr_t)taskInfo);  // r25
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