#include "Buratino.h"

BDigitalPin led(LED_BUILTIN, BPinMode::Output, BPinTrigger::Never);

// synchronize access to this shared global variable
BSync<bool> dash = true;

void Dots(Buratino* a, void* b) {

  while(1) {
    noInterrupts();
    SerialUSB.println("dots");
    interrupts();
    Buratino::YieldTask();

    // if (!dash) {
    //   auto ct=3;
    //   while(ct-->0) {
    //     led(HIGH);
    //     delay(200);
    //     led(LOW);
    //     delay(200);
    //   }
    //   dash = true;
    // }
  }
}

void Dashes(Buratino* a, void* b) {
  while(1) {
    noInterrupts();
    SerialUSB.println("dashes");
    interrupts();
    Buratino::YieldTask();
  //  if (dash) {
  //     auto ct=3;
  //     while(ct-->0) {
  //       led(LOW);
  //       //delay(700);
  //       led(HIGH);
  //       //(700);
  //     }
  //    dash = false;
  //  }
  }
}

void setup() {
  SerialUSB.begin(115200);
  noInterrupts();
  led.Reset(1);
  Buratino::Setup(1 /* number of tasks */);  // also sets up task switcher interrupt
  Buratino::RunTask(BTask(Dashes), 0, 2024);
  Buratino::RunTask(BTask(Dots), 0, 2024);
  interrupts();
}

void loop() {
  Buratino::YieldTask();
  noInterrupts();
  SerialUSB.println("loop"); //delay(1000);
  interrupts();
  //Buratino::YieldTask();
}