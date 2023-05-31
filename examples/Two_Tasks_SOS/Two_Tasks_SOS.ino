// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
#include "BSync.h"

BSync<bool> dashes(false);
#define SerialUSB Serial
void Dots(int b) {
  while (1) {
    SerialUSB.println("dots");
    delay(500);
  }
}

void Dashes(int a) {
  while (1) {
    SerialUSB.println("dashes");
    delay(500);
  }
}

void setup() {
  SerialUSB.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  setupTasks(2);
  noInterrupts();
  runTask(Dots, 0, 0, 640);
  runTask(Dashes, 0, 2, 640);
  interrupts();
}

void loop() {
  SerialUSB.println("loop");
  delay(500);
}