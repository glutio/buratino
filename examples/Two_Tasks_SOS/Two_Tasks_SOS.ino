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
  while(1)
  {
    led(0); delay(700); led(1);  delay(700);
    led(0); delay(700); led(1);  delay(700);
    led(0); delay(700); led(1);  delay(700);

    led(0); delay(200); led(1);  delay(200);
    led(0); delay(200); led(1);  delay(200);
    led(0); delay(200); led(1);  delay(200);

    delay(700);
  }

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
}

int i =0;
  BTask task(Task1);
void loop() {
  delay(10000);
  auto t = B::run_task(task,(void*)123, 256);  
  SerialUSB.println(i++);
  B::switch_task();
  B::kill_task(t);

  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);
  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);
  // digitalWrite(LED_BUILTIN, 0); delay(700); digitalWrite(LED_BUILTIN, 1);  delay(700);

  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);
  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);
  // digitalWrite(LED_BUILTIN, 0); delay(200); digitalWrite(LED_BUILTIN, 1);  delay(200);

  // delay(700);
}
