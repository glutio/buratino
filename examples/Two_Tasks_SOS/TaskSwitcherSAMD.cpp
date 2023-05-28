#ifdef ARDUINO_ARCH_SAMD
#include <Arduino.h>
#include "BTaskSwitcher.h"

struct Ctx {
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t psr;
};

unsigned BTaskSwitcher::context_size() {
  return sizeof(Ctx);
}

bool BTaskSwitcher::disable() {
  auto enabled = __get_PRIMASK() == 0;
  noInterrupts();
  return enabled;
}

void BTaskSwitcher::init_task(BTaskInfoBase* taskInfo, BTaskWrapper wrapper) {
  // 8 bytes align per ARM Cortex+ requirement when entering interrupt
  taskInfo->sp = (uint8_t*)((uintptr_t)taskInfo->sp & ~0x7);

  // clear registers
  for (unsigned i = 0; i < sizeof(Ctx); ++i) {
    *--taskInfo->sp = 0;
  }

  auto ctx = (Ctx*)taskInfo->sp;
  // compiler/architecture specific, passing argument via registers
  ctx->r0 = (uintptr_t)taskInfo;
  ctx->psr = 0x01000000;
  ctx->pc = (uintptr_t)wrapper;
}

void BTaskSwitcher::switch_context() {
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void BTaskSwitcher::init_arch() {
  _next_task = _current_task;
}

extern "C" {

  void __attribute__((naked)) PendSV_Handler() {
    asm volatile("push {r4-r7}");
    asm volatile("mov r4,r8");
    asm volatile("mov r5,r9");
    asm volatile("mov r6,r10");
    asm volatile("mov r7,r11");
    asm volatile("push {r4-r7}");

    asm volatile("mov r0, sp");
    asm volatile("push {lr}");
    asm volatile("blx %0"
                 :
                 : "r"(BTaskSwitcher::swap_stack)
                 : "r0");
    asm volatile("mov r12, r0");
    asm volatile("pop {r0}");
    asm volatile("mov lr, r0");

    asm volatile("mov sp, r12");
    asm volatile("pop {r4-r7}");
    asm volatile("mov r8,r4");
    asm volatile("mov r9,r5");
    asm volatile("mov r10,r6");
    asm volatile("mov r11,r7");
    asm volatile("pop {r4-r7}");

    asm volatile("bx lr");
  }

  int sysTickHook() {
    auto sreg = BTaskSwitcher::disable();
    BTaskSwitcher::schedule_task();
    BTaskSwitcher::restore(sreg);
    return 0;
  }
}

#endif
