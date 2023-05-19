#ifdef ARDUINO_ARCH_SAMD

#include <Arduino.h>
#include "BTaskSwitcher.h"

namespace B {

void switch_task();
void task_wrapper(BTaskInfo*);
void restore(bool);

enum Ctx {
  r3,
  lr,
  primask,
  psr,
  r0,
  r1,
  r2,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  size
};

int8_t __attribute__((inline)) context_size() {
  return Ctx::size * sizeof(uint32_t);
}

bool disable() {
  auto enabled = interruptsStatus();
  noInterrupts();
  return enabled;
}

void __attribute__((naked)) switch_context(uint8_t** old_sp, uint8_t* new_sp) {
  asm volatile("push {r3, lr}");
  asm volatile("mrs r3, primask");
  asm volatile("push {r3}");
  asm volatile("mrs r3, psr");
  asm volatile("push {r3}");
  asm volatile("push {r0, r1, r2, r4, r5, r6, r7}");
  asm volatile("mov r3,r8");
  asm volatile("mov r4,r9");
  asm volatile("mov r5,r10");
  asm volatile("mov r6,r11");
  asm volatile("mov r7,r12");
  asm volatile("push {r3-r7}");
  asm volatile("mov r3, sp");
  asm volatile("str r3, [r0]");

  asm volatile("mov sp, r1");
  asm volatile("pop {r3-r7}");
  asm volatile("mov r8,r3");
  asm volatile("mov r9,r4");
  asm volatile("mov r10,r5");
  asm volatile("mov r11,r6");
  asm volatile("mov r12,r7");
  asm volatile("pop {r0, r1, r2, r4, r5, r6, r7}");
  asm volatile("pop {r3}");
  asm volatile("msr psr, r3");
  asm volatile("pop {r3}");
  asm volatile("msr primask, r3");
  asm volatile("pop {r3, pc}");
}

int init_task(BTaskInfo* taskInfo) {
  // align stack pointer and skip a word
  taskInfo->sp = (uint8_t*)((uintptr_t)taskInfo->sp & ~0x7);

  // clear registers
  for (unsigned i = 0; i < Ctx::size; ++i) {
    *(uint32_t*)taskInfo->sp = 99-i;
    taskInfo->sp -= sizeof(uint32_t);
  }

  // // compiler/architecture specific, passing argument via registers
  uint32_t* sp = (uint32_t*)taskInfo->sp;
  SerialUSB.printf("init: %u\n", taskInfo);
  sp[Ctx::size - Ctx::r0] = (uintptr_t)taskInfo;
  sp[Ctx::size - Ctx::psr] = 0x1F | 0x40;  // sys mode and disable fiq
  sp[Ctx::size - Ctx::primask] = 0x1;
  sp[Ctx::size - Ctx::lr] = (uintptr_t)task_wrapper;  // r24

  // for (unsigned i = 0; i < Ctx::size; i++) {
  //   SerialUSB.print(sp[Ctx::size], )
  // }
}

void init_arch() {
}

}

extern "C" {
  int sysTickHook() {
    auto i = B::disable();
    // B::switch_task();
    B::restore(i);
    return 0;
  }
}

#endif
