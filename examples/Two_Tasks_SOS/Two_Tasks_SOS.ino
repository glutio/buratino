#include "Buratino.h"

BDigitalPin led(LED_BUILTIN, BPinMode::Output, BPinTrigger::Never);

// synchronize access to this shared global variable
BSync<bool> dash = true;

void Dots(Buratino* a, void* b) {
  while(1) {
    if (!dash) {
      auto ct=3;
      while(ct-->0) {
        led(HIGH);
        delay(200);
        led(LOW);
        delay(200);
      }
      dash = true;
    }
  }
}

void Dashes(Buratino* a, void* b) {
  while(1) {
    if (dash) {
      auto ct=3;
      while(ct-->0) {
        led(HIGH);
        delay(700);
        led(LOW);
        delay(700);
      }
      dash = false;
    }
  }
}

void setup() {
  noInterrupts();
  led.Reset();
  Buratino::Setup(1 /* number of tasks */);  // also sets up task switcher interrupt
  Buratino::RunTask(BTask(Dashes), 0, 512);
  Buratino::RunTask(BTask(Dots), 0, 512);
  interrupts();
}

void loop() {
  Buratino::YieldTask();
}
