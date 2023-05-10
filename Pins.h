#ifndef __PINS_H__
#define __PINS_H__

#include "Bind.h"
#include "Device.h"

struct AnalogPinChangeArgs {
  int16_t oldValue;
  int16_t value;
};

struct DigitalPinChangeArgs {
  int8_t oldValue;
  int8_t value;
};

enum PinTrigger 
{
  Change,
  Low,
  High,
  Always
};

class AnalogPin: public IBuratinoDevice {
protected:
  int8_t _pin;
  int16_t _value;
  int8_t _trigger;
public:
  typedef EventDelegate<AnalogPin, AnalogPinChangeArgs> ChangeEvent;
  ChangeEvent OnChange;

public:
  AnalogPin(int8_t pin, PinTrigger trigger = PinTrigger::Change)
    : _pin(pin) {}

  int16_t get_Value()
  {
    return _value;
  }

  virtual void Setup() {
    _value = analogRead(_pin);
  }

  virtual void Update() {
    auto value = _value;
    _value = analogRead(_pin);

    if ((_trigger == PinTrigger::Change && value != _value) ||
        (_trigger == PinTrigger::Low && _value == 0) ||
        (_trigger == PinTrigger::High && _value == 1) || 
        (_trigger == PinTrigger::Always)) {
      AnalogPinChangeArgs args;
      args.value = _value;
      args.oldValue = value;
      OnChange(this, &args);
    }
  }
};

class DigitalPin: public IBuratinoDevice {
protected:
  int8_t _pin;
  int8_t _value;
  int8_t _pinMode;
  PinTrigger _trigger;

public:
  typedef EventDelegate<DigitalPin, DigitalPinChangeArgs> ChangeEvent;
  ChangeEvent OnChange;

public:
  int8_t get_Value()
  {
    return _value;
  }

  DigitalPin(int8_t pin, int8_t pinMode, PinTrigger trigger)
    : _pin(pin), _pinMode(pinMode), _trigger(trigger) {}

  virtual void Setup() {
    pinMode(_pin, _pinMode);
    _value = digitalRead(_pin);
  }

  virtual void Update() {
    auto value = _value;
    _value = digitalRead(_pin);
    if ((_trigger == PinTrigger::Change && value != _value) ||
        (_trigger == PinTrigger::Low && _value == 0) ||
        (_trigger == PinTrigger::High && _value == 1) || 
        (_trigger == PinTrigger::Always)) {
      DigitalPinChangeArgs args;
      args.value = _value;
      args.oldValue = value;
      OnChange(this, &args);
    }
  }
};

#endif