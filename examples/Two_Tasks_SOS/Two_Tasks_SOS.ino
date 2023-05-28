// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
#include "BSync.h"

BSync<bool> dashes(false);

void Dots(int b) {
  while (1) {
    //SerialUSB.println("dots");
    delayTask(500);
  }
}

void Dashes(int a) {
  while (1) {
    //SerialUSB.println("dashes");
    delayTask(500);
  }
}

void setup() {
  //SerialUSB.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  setupTasks(2);
  noInterrupts();
  runTask(Dots, 0, 0, 1024 * 2);
  runTask(Dashes, 0, 2, 1024 * 2);
  interrupts();
}

void loop() {
  //SerialUSB.println("loop");
  delayTask(500);
}