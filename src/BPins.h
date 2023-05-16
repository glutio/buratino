#ifndef __BPINS_H__
#define __BPINS_H__

#include <Arduino.h>
#include "BEvent.h"

struct BAnalogPinChangeArgs {
  int16_t oldValue;
  int16_t value;
};

struct BDigitalPinChangeArgs {
  int8_t oldValue;
  int8_t value;
};

enum BPinTrigger {
  Change,
  Low,
  High,
  Always,
  Never
};

enum BPinMode {
  Input = INPUT,
  Output = OUTPUT,
  InputPullup = INPUT_PULLUP
};

class BAnalogPin {
protected:
  uint8_t _pin;
  uint16_t _value;
  uint8_t _threshold;
  BPinTrigger _trigger;

public:
  typedef BEvent<BAnalogPin, BAnalogPinChangeArgs> ChangeEvent;
  ChangeEvent OnChange;

public:
  BAnalogPin(uint8_t pin, BPinTrigger trigger = BPinTrigger::Change, uint8_t threshold = 15);
  uint16_t Value();
  void Reset();
  void Update();

  operator uint16_t();
};

class BDigitalPin {
protected:
  uint8_t _pin;
  uint8_t _value;
  BPinMode _pinMode;
  BPinTrigger _trigger;

public:
  typedef BEvent<BDigitalPin, BDigitalPinChangeArgs> ChangeEvent;
  ChangeEvent OnChange;

public:
  BDigitalPin(uint8_t pin, BPinMode pinMode = BPinMode::InputPullup, BPinTrigger trigger = BPinTrigger::Change);
               
  uint8_t Value();
  void Reset();
  void Update();

  operator()(uint8_t value);
  operator uint8_t();
  explicit operator bool();
};

#endif