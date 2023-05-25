// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
//#include "BSync.h"

#define SerialUSB Serial

//BSync<bool> dashes(false);

void Dots(int b) {
  while (1) {
    //if (!dashes) {
      // delay(1000);
      // digitalWrite(LED_BUILTIN, LOW);
      // delay(1000);
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(1000);
      // digitalWrite(LED_BUILTIN, LOW);
      // delay(1000);
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      //dashes = true;
    //}
  }
}

void Dashes(int b) {
  while (1) {
    //if (dashes) {
      // delay(200);
      // digitalWrite(LED_BUILTIN, LOW);
      // delay(200);
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(200);
      // digitalWrite(LED_BUILTIN, LOW);
      // delay(200);
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(200);
      // digitalWrite(LED_BUILTIN, LOW);
      // delay(200);
      // digitalWrite(LED_BUILTIN, HIGH);
      // dashes = false;
    //}
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // SerialUSB.begin(115200);
  noInterrupts();
  setupTasks(2);
  runTask(Dots, 0, 0, 2048);
  runTask(Dashes, 0, 0, 2048);
  interrupts();
  //delay(1000);
}

void loop() {

}