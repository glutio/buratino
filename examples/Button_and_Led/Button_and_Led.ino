#include "Buratino.h"

BDigitalPin button(9 /* pin */, BPinMode::InputPullup /* pinMode */, BPinTrigger::Change /* trigger event on value change */);
BDigitalPin led(6, BPinMode::Output, BPinTrigger::Never /* output pin never triggers event */);

void OnButtonChange(BDigitalPin* sender, BDigitalPinChangeArgs* args)
{
  // Write to digital output. Use oldValue because the InputPullup button sends 0 on press, so OnChange the old value is 1
  led(args->oldValue);
}

void setup() {
  // put your setup code here, to run once:
  button.OnChange = BDigitalPin::ChangeEvent(OnButtonChange);
}

void loop() {
  // put your main code here, to run repeatedly:
  button.Update(); // read pin value
}
