#include <Arduino.h>
#include "Timer.h"

TimerOne TimerOne::instance;

TimerOne::TimerOne() {
}

void TimerOne::Setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0xF424 / 4;
  TCCR1B = (1 << WGM12) | (1 << CS12);
  TIMSK1 = (1 << OCIE1A);
  sei();
}

void TimerOne::Tick(TimerArgs* args) {
  OnTick(this, args);
}

void ISR_Wrapper(TimerArgs* args) {  
  TimerOne::instance.Tick(args);
}

ISR(TIMER1_COMPA_vect) {  
  TimerArgs args;
  asm volatile(
    "push r28\n\t"        // Save Y-register low byte (r28)
    "push r29\n\t"        // Save Y-register high byte (r29)
    "in r28, __SP_L__\n\t" // Load low byte of stack pointer (SPL) into r28 (Y-register low byte)
    "in r29, __SP_H__\n\t" // Load high byte of stack pointer (SPH) into r29 (Y-register high byte)
    "adiw r28, %[offset]\n\t" // Add localVarOffset to Y-register
    "ld %A0, Y+\n\t"        // Load the return address low byte from the stack to pc_low and increment Y
    "ld %B0, Y\n\t"         // Load the return address high byte from the stack to pc_high
    "pop r29\n\t"         // Restore Y-register high byte (r29)
    "pop r28\n\t"         // Restore Y-register low byte (r28)
    : "=r" (args.pc)
    : [offset] "M" (sizeof(args) + 2) // 2 for saving r28 and r29
  ); 
  ISR_Wrapper(&args);
}