#include "Buratino.h"

BJoystick joystick(1 /* x axis analog pin */, 2 /* y axis */, 2 /* switch digital pin */);
BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never);

int16_t timeout = 1000;
void TimerTask(Buratino* a, void* b) {  
  while (1) {
    led(HIGH);

    // read shared variable into local
    auto sreg = SREG;
    noInterrupts();
    auto localValue = timeout;
    SREG = sreg;
    Serial.println(localValue);
    delay(localValue);
    led(LOW);
    Buratino::YieldTask();
    noInterrupts();
    localValue = timeout;
    SREG = sreg;
    delay(localValue);
  }
}

void OnJoystickMove(BJoystick* joystick, BJoystickMoveArgs* args) {
  auto sreg = SREG;
  noInterrupts();
  timeout = map((long)args->Y * args->X, 0, 1023*1023, 100, 2000);
  SREG = sreg;
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  joystick.Calibrate();
  joystick.OnMove = BJoystick::MoveEvent(OnJoystickMove);
  noInterrupts();
  Buratino::Setup(1 /* number of tasks */);
  Buratino::RunTask(BTask(TimerTask), 0, 1024);
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:
  joystick.Update();  // read pin values
  delay(100);
}
