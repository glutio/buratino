#ifdef ARDUINO_ARCH_SAMD

#include <Arduino.h>
#include "BTaskSwitcher.h"
#include "BList.h"

namespace B {

int next_task;

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

bool disable() {
  auto enabled = interruptsStatus();
  noInterrupts();
  return enabled;
}

int init_task(BTaskInfo* taskInfo) {
  // 8 bytes align per ARM Cortex+ requirement when entering interrupt
  uint32_t* sp = (uint32_t*)((uintptr_t)taskInfo->sp & ~0x7);

  // clear registers
  for (unsigned i = 0; i < sizeof(Ctx) / sizeof(uint32_t); ++i) {
    *--sp = 0;
  }
  taskInfo->sp = (uint8_t*)sp;

  auto ctx = (Ctx*)sp;
  // compiler/architecture specific, passing argument via registers
  ctx->r0 = (uintptr_t)taskInfo;
  ctx->psr = 0x01000000;
  ctx->pc = (uintptr_t)task_wrapper;  // r24
}

void yield_task() {
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void kill_task(int id) {
  auto sreg = disable();
  if (id > 0 && id < _tasks.Length() && _tasks[id]) {
    if (id != current_task) {
      free_task(id);
    } else {
      _tasks[id]->id = -1;
    }
    yield_task();
  }
  restore(sreg);
}

void init_arch() {
  next_task = current_task;
}

}

extern "C" {
  uint8_t* switch_task(uint8_t* sp) {
    using namespace B;
    _tasks[current_task]->sp = sp;
    if (_tasks[current_task]->id < 0) {
      free_task(current_task);
    }
    current_task = next_task;
    sp = _tasks[current_task]->sp;
    return sp;
  }

  void __attribute__((naked)) PendSV_Handler() {
    asm volatile("cpsid	i");
    asm volatile("push {r4-r7}");
    asm volatile("mov r4,r8");
    asm volatile("mov r5,r9");
    asm volatile("mov r6,r10");
    asm volatile("mov r7,r11");
    asm volatile("push {r4-r7}");

    asm volatile("mov r0, sp");
    asm volatile("push {lr}");
    asm volatile("bl switch_task");
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

    asm volatile("cpsie	i");
    asm volatile("bx lr");
  }

  int sysTickHook() {
    //  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    return 0;
  }
}

#endif
