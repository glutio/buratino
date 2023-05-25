// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
#define SerialUSB Serial

void Dots(int b) {

  while(1) {
    noInterrupts();
    SerialUSB.println("dots");
    interrupts();
    delay(1000);
    //Buratino::YieldTask();

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

void Dashes(int b) {
  while(1) {
    noInterrupts();
    SerialUSB.println("dashes");
    interrupts();
    delay(1000);
    //Buratino::YieldTask();
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
  //SerialUSB.begin(115200);
  noInterrupts();
  setupTasks(2);
  runTask(Dots, 0, 0, 640);
  runTask(Dashes, 0, 0, 640);
  interrupts();
  //delay(1000);
}

void loop() {
  //Buratino::YieldTask();
  noInterrupts();
  SerialUSB.println("loop");
  interrupts();
  delay(1000);
  //Buratino::YieldTask();
}