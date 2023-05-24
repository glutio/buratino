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

unsigned BTaskSwitcher::context_size()
{
  return sizeof(Ctx);
}

bool BTaskSwitcher::disable() {
  auto enabled = interruptsStatus();
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

void BTaskSwitcher::schedule_task() {
  auto next_task = get_next_task();
  if (next_task == _current_task) {
    return;
  }
  _next_task = next_task;
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void BTaskSwitcher::yield_task() {
  asm volatile("push {lr}");
  asm volatile("mrs r3, psr");
  asm volatile("push {r3}");  //psr
  asm volatile("ldr r3, =_Return");
  asm volatile("push {r3}");  // pc
  asm volatile("push {r3}");  // lr
  asm volatile("push {r3}");  // r12
  asm volatile("push {r0-r3}");
  asm volatile("bl PendSV_Handler");
  // //schedule_task();
  asm volatile("pop {r0-r3}");
  asm volatile("pop {r3}");  // r12
  asm volatile("pop {r3}");  // lr
  asm volatile("pop {r3}");  // pc
  asm volatile("mov r0, r3");
  asm volatile("pop {r3}");  // psr
  asm volatile("msr psr, r3");
  asm volatile("pop {r3}");  // real lr
  asm volatile("mov lr, r3");
  asm volatile("mov pc, r0");
  asm volatile("_Return:");
  return;
}

void BTaskSwitcher::kill_task(int id) {
  auto sreg = disable();
  if (id > 0 && id < _tasks.Length() && _tasks[id]) {
    if (id != _current_task) {
      free_task(id);
    } else {
      _tasks[id]->id = -1;
    }
    yield_task();
  }
  restore(sreg);
}

void BTaskSwitcher::init_arch() {
  _next_task = _current_task;
}

uint8_t* BTaskSwitcher::switch_task(uint8_t* sp) {
  if (_tasks[_current_task]->id < 0) {
    free_task(_current_task);
  } else {
    _tasks[_current_task]->sp = sp;
  }
  _current_task = _next_task;
  sp = _tasks[_current_task]->sp;
  return sp;
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
    asm volatile("bx %0" : : "r"(BTaskSwitcher::switch_task) : "r0");
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
    BTaskSwitcher::schedule_task();
    return 0;
  }
}

#endif
