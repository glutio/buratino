// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
#include "BSync.h"

BSync<bool> dashes(false);

void Dots(int b) {
  while (1) {
     if (!dashes) {
    //   delay(1000);
       digitalWrite(LED_BUILTIN, HIGH);
       delay(1000);
       dashes = true;
     }
  }
}

void Dashes(int a) {
  while (1) {
     if (dashes) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
       dashes = false;
     }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  noInterrupts();
  setupTasks(2);
  
  runTask(Dots, 0, 0, 512);
  runTask(Dashes, 0, 2, 512);
  interrupts();
  //delay(1000);
}

void loop() {
  delay(5000);
}