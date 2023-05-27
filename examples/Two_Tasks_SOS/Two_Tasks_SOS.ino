// synchronize access to this shared global variable
#include "BTaskSwitcher.h"
#include "BSync.h"

BSync<bool> dashes(false);

void Dots(int b) {
  //Serial.println("dots");
  while(1)
  {
        noInterrupts();
      Serial.println("dots");
      interrupts();
      delay(1500);

  }
}

void Dashes(int a) {
    // noInterrupts();
  // BTaskSwitcher::_initialized=false;
  // interrupts();
  // digitalWrite(LED_BUILTIN, LOW);
  dashes=true;
  while(1) {
    noInterrupts();
      Serial.println("dashes");
      interrupts();
  delay(1500);
  };
}

void setup() {
  SerialUSB.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  noInterrupts();
  setupTasks(2);
  
  runTask(Dots, 0, 0, 1024*5);
  runTask(Dashes, 0, 2, 1024*5);
  interrupts();
  //delay(1000);
}

void loop() {
 if (dashes) {
  // noInterrupts();
  // BTaskSwitcher::_initialized=false;
  // interrupts();
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
 }
}