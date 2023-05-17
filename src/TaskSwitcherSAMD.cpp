#ifdef ARDUINO_ARCH_SAMD
#include <Arduino.h>
#include "BTaskSwitcher.h"

namespace B {

void switch_task();
void task_wrapper(BTaskInfo*);

enum Ctx {
  lr,
  r3,
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
  return Ctx::size;
}

bool disable() {
  auto enabled = interruptsStatus();
  noInterrupts();
  return enabled;
}

void __attribute__((naked)) switch_context(uint8_t** old_sp, uint8_t* new_sp) {
  asm volatile("push {r3, lr}");
  asm volatile("mrs r3, psr");
  asm volatile("push {r3}");
  asm volatile("push {r0, r1, r2, r4, r5, r6, r7}");
  asm volatile("mov r3,r8");
  asm volatile("mov r4,r9");
  asm volatile("mov r5,r10");
  asm volatile("mov r6,r11");
  asm volatile("mov r7,r12");
  asm volatile("push {r3-r7}");
  asm volatile("mrs r3, psp");
  asm volatile("str r3, [r0]");

  asm volatile("ldr r3, [r1]");
  asm volatile("msr psp, r3");
  asm volatile("pop {r3-r7}");
  asm volatile("mov r8,r3");
  asm volatile("mov r9,r4");
  asm volatile("mov r10,r5");
  asm volatile("mov r11,r6");
  asm volatile("mov r12,r7");
  asm volatile("pop {r0, r1, r2, r4, r5, r6, r7}");
  asm volatile("pop {r3}");
  asm volatile("msr psr, r3");
  asm volatile("pop {r3, pc}");
}

int init_task(BTaskInfo* taskInfo) {
  // align
  if ((uintptr_t)taskInfo->sp & 0x4) {
    --taskInfo->sp;
  }

  // clear registers
  for (unsigned i = 0; i < Ctx::size; ++i) {
    *taskInfo->sp-- = 0;
  }

  // compiler/architecture specific, passing argument via registers
  *(taskInfo->sp + Ctx::size - Ctx::r0) = (uintptr_t)taskInfo;
  *(taskInfo->sp + Ctx::size - Ctx::psr) = 0x1F | 0x40;             // sys mode and disable fiq
  *(taskInfo->sp + Ctx::size - Ctx::lr) = (uintptr_t)task_wrapper;  // r24
}

void init_arch() {
  // Configure GCLK1 to use OSC8M
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |  // Divide the 8MHz clock source by divisor 1: 8MHz/1=8MHz
                     GCLK_GENDIV_ID(1);    // Select Generic Clock (GCLK) 1
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // Wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |        // Set the duty cycle to 50/50 HIGH/LOW
                      GCLK_GENCTRL_GENEN |      // Enable GCLK1
                      GCLK_GENCTRL_SRC_OSC8M |  // Set the 8MHz oscillator as the clock source
                      GCLK_GENCTRL_ID(1);       // Select GCLK1
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // Wait for synchronization

  // Feed GCLK1 to TCC1 and TCC0
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable GCLK1 to TCC1 and TCC0
                      GCLK_CLKCTRL_GEN_GCLK1 |    // Select GCLK1
                      GCLK_CLKCTRL_ID_TCC0_TCC1;  // Feed the GCLK1 to TCC1 and TCC0
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // Wait for synchronization

  // Configure TCC1
  TCC1->CTRLA.reg = TC_CTRLA_PRESCALER_DIV8 |  // Set prescaler to 8, 8MHz/8 = 1MHz
                    TC_CTRLA_PRESCSYNC_PRESC;  // Set the reset/reload to trigger on prescaler clock
  while (TCC1->SYNCBUSY.bit.ENABLE)
    ;  // Wait for synchronization

  TCC1->PER.reg = 1000;  // Set the frequency of the PWM on TCC1 to 1kHz
  while (TCC1->SYNCBUSY.bit.PER)
    ;  // Wait for synchronization

  // Enable the TCC1 interrupt
  NVIC_EnableIRQ(TCC1_IRQn);

  TCC1->INTENSET.reg = TCC_INTENSET_OVF;  // Enable overflow interrupt
  while (TCC1->SYNCBUSY.bit.ENABLE)
    ;  // Wait for synchronization

  TCC1->CTRLA.reg |= TCC_CTRLA_ENABLE;  // Enable TCC1
  while (TCC1->SYNCBUSY.bit.ENABLE)
    ;
}

void TCC1_Handler() {
  // Check for overflow (OVF) interrupt
  if (TCC1->INTFLAG.bit.OVF && TCC1->INTENSET.bit.OVF) {
    // Place the interrupt handler code here
    switch_task();
    // Clear the OVF interrupt flag
    TCC1->INTFLAG.bit.OVF = 1;
  }
}
}

#endif
