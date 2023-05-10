#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "Bind.h"
#include "Pins.h"
#include "Buratino.h"

class Joystick;
class JoystickMoveArgs;

struct JoystickMoveArgs {
  int16_t oldX;
  int16_t oldY;
  int16_t X;
  int16_t Y;
};

class Joystick : public IBuratinoDevice {
public:
  typedef EventDelegate<Joystick, JoystickMoveArgs> MoveEvent;
  typedef EventDelegate<Joystick, void> ClickEvent;

protected:
  int8_t _xPin;
  int8_t _yPin;
  int8_t _swPin;

  int16_t _x;
  int16_t _y;
  bool _sw;

public:
  MoveEvent OnMove;
  ClickEvent OnClick;

public:
  Joystick(int8_t x, int8_t y, int8_t sw)
    : _xPin(x), _yPin(y), _swPin(sw)
  {
  }

  virtual void Setup() {
    pinMode(_swPin, INPUT_PULLUP);
  }

  virtual void Update() {
    auto x = _x;
    auto y = _y;
    _x = analogRead(_xPin);
    _y = analogRead(_yPin);

    if (_x < 500 || _x > 524 || _y < 500 || _y > 524) {
      JoystickMoveArgs args;
      args.oldX = x;
      args.oldY = y;
      args.X = _x;
      args.Y = _y;
      OnMove(this, &args);
    }

    auto sw = _sw;
    _sw = digitalRead(_swPin);
    if (!_sw && sw) {
      OnClick(this, 0);
    }
  }
};

#endif