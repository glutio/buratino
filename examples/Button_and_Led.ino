#include "Buratino.h"

BDigitalPin button(9, BPinMode::InputPullup, BPinTrigger::Change);
BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never);

void OnButtonChange(BDigitalPin* sender, BDigitalPinChangeArgs* args)
{
  led(args->oldValue);
}

void setup() {
  // put your setup code here, to run once:
  button.OnChange = BDigitalPin::ChangeEvent(OnButtonChange);
}

void loop() {
  // put your main code here, to run repeatedly:
  button.Update();
}
