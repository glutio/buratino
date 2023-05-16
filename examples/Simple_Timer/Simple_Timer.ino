#include "Buratino.h"

BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never);

class Application {
public:
  void TimerTask(Buratino*, void*) {
    while(1) {
      delay(1000);
      led(!led);
    }
  }

  void Setup() {
    Buratino::RunTask(BTask(this, &Application::TimerTask), 0, 256 /* stackSize */);
  }
};

Application app;

void setup() {
  led.Reset();
  noInterrupts();
  Buratino::Setup(1);
  app.Setup();
  interrupts();
}

void loop() {
}