#include "BPins.h"

/*
  BAnalogPin - represents an analog pin on an Arduino board
*/
BAnalogPin::BAnalogPin(uint8_t pin, BPinTrigger trigger, uint8_t threshold)
  : _pin(pin), _trigger(trigger), _threshold(threshold) {
  Reset();
}

uint16_t BAnalogPin::Value() {
  return _value;
}

void BAnalogPin::Reset() {
  _value = analogRead(_pin);
}

void BAnalogPin::Update() {
  auto value = analogRead(_pin);

  _value = analogRead(_pin);

  if ((_trigger == BPinTrigger::Change && _value + _threshold > value && _value - _threshold < value) || (_trigger == BPinTrigger::Low && value < _threshold) || (_trigger == BPinTrigger::High && value > _threshold) || (_trigger == BPinTrigger::Always)) {
    BAnalogPinChangeArgs args;
    args.value = value;
    args.oldValue = _value;
    _value = value;
    OnChange(this, &args);
  }
}

BAnalogPin::operator uint16_t() {
  return Value();
}

/*
  BDigitalPin - represents a digital pin on an Arduino board
*/
BDigitalPin::BDigitalPin(uint8_t pin, BPinMode pinMode, BPinTrigger trigger)
  : _pin(pin), _pinMode(pinMode), _trigger(trigger) {
  Reset();
}

uint8_t BDigitalPin::Value() {
  return _value;
}

void BDigitalPin::Reset() {
  pinMode(_pin, _pinMode);
  if (_pinMode != BPinMode::Output) {
    _value = digitalRead(_pin);
  }
}

void BDigitalPin::Update() {
  auto value = _value;
  _value = digitalRead(_pin);
  if ((_trigger == BPinTrigger::Change && value != _value) || (_trigger == BPinTrigger::Low && _value == LOW) || (_trigger == BPinTrigger::High && _value == HIGH) || (_trigger == BPinTrigger::Always)) {
    BDigitalPinChangeArgs args;
    args.value = _value;
    args.oldValue = value;
    OnChange(this, &args);
  }
}

BDigitalPin::operator()(uint8_t value) {
  if (_pinMode == BPinMode::Output) {
    digitalWrite(_pin, value);
    _value = value;
  }
}

BDigitalPin::operator uint8_t() {
  return Value();
}
