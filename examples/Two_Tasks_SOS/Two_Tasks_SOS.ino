#include "Buratino.h"
BDigitalPin led(LED_BUILTIN, BPinMode::Output, BPinTrigger::Never);

// // synchronize access to this shared global variable
// BSync<bool> dash = true;

// void Dots(Buratino* a, void* b) {
//   while(1) {
//     if (!dash) {
//       auto ct=3;
//       while(ct-->0) {
//         led(HIGH);
//         delay(200);
//         led(LOW);
//         delay(200);
//       }
//       dash = true;
//     }
//   }
// }

// void Dashes(Buratino* a, void* b) {
//   while(1) {
//     if (dash) {
//       auto ct=3;
//       while(ct-->0) {
//         led(HIGH);
//         delay(700);
//         led(LOW);
//         delay(700);
//       }
//       dash = false;
//     }
//   }
// }

void Task1(Buratino*, void*)
{
  int x=-1;
  asm volatile ("ldr r0,[%0]"::"r"(x):);
  SerialUSB.println("task1");
  SerialUSB.flush();
  delay(1000);
}

namespace B{
  int run_task(BTask& task, BTask::ArgumentType* arg, uintptr_t stackSize) ;
  void kill_task(int);
  void switch_task();
};

void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);
  SerialUSB.begin(115200);
  led.Reset(1);
  Buratino::Setup(1);
  SerialUSB.println("done");
  SerialUSB.flush();
  BTask task(Task1);
  auto t = B::run_task(task,0, 256);  
}



void loop() {
  //B::switch_task();
  led(0); delay(700); led(1);  delay(700);
  led(0); delay(700); led(1);  delay(700);
  led(0); delay(700); led(1);  delay(700);

  led(0); delay(200); led(1);  delay(200);
  led(0); delay(200); led(1);  delay(200);
  led(0); delay(200); led(1);  delay(200);
  //B::kill_task(t);

  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);
  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);
  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);

  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);
  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);
  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);


  delay(700);

}
