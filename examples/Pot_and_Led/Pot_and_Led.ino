#include "Buratino.h"

BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never);
BAnalogPin pot(0 /* pin */);

int16_t timeout = 1000;
void TimerTask(Buratino* a, void* b) {  
  auto start = millis();
  while (1) {
    // disable task switching while reading shared global variable into local variable
    auto sreg = SREG; // save state register (where interrupts flag is)
    noInterrupts(); // disable interrupts (clear the flag)
    auto local = timeout; // save global value locally
    SREG = sreg; // return to whatever state interrupt flag was before

    if (millis() - start > local)
    {
      led(!led);
      start = millis();
      Buratino::YieldTask(); // done with this iteration, let other tasks run
    }
  }
}

void OnPotChange(BAnalogPin* pin, BAnalogPinChangeArgs* args) {
  auto sreg = SREG;
  noInterrupts();
  timeout = args->value;
  SREG = sreg;
}

void setup() {
  pot.Reset();
  pot.OnChange = BAnalogPin::ChangeEvent(OnPotChange);
  noInterrupts(); // disable interrupts (while setting up tasks)
  Buratino::Setup(1 /* number of tasks */); // sets up time switcher interrupt
  Buratino::RunTask(BTask(TimerTask), 0, 1024);
  interrupts(); // enable task switcher
}

void loop() {
  // put your main code here, to run repeatedly:
  pot.Update(); // refresh values from inputs
  delay(100);
}
