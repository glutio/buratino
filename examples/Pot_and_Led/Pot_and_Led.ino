#include "Buratino.h"

BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never);
BAnalogPin pot(0 /* pin */);

BSync<uint16_t> timeout(1000);

void TimerTask(Buratino* a, void* b) {  
  auto start = millis();
  while (1) {
    // disable task switching while reading shared global variable into local variable
    if (timeout < (uint16_t)(millis() - start))
    {
      led(!led);
      start = millis();
      Buratino::YieldTask(); // done with this iteration, let other tasks run
    }
  }
}

void OnPotChange(BAnalogPin* pin, BAnalogPinChangeArgs* args) {
  timeout = args->value;
}

void setup() {
  pot.Reset();
  pot.OnChange = BAnalogPin::ChangeEvent(OnPotChange);
  
  Buratino::Stop();
  Buratino::Setup(1 /* number of tasks */); // sets up time switcher interrupt
  Buratino::Start();
  Buratino::RunTask(BTask(TimerTask), 0, 1024);
  interrupts(); // enable task switcher
}

void loop() {
  // put your main code here, to run repeatedly:
  pot.Update(); // refresh values from inputs
  delay(100);
}
