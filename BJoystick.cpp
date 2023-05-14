#ifndef __BJOYSTICK_H__
#define __BJOYSTICK_H__

#include <Arduino.h>

#include "BJoystick.h"

BJoystick::BJoystick(uint8_t x, uint8_t y, uint8_t sw, uint16_t centerX, uint16_t centerY)
  : _xPin(x), _yPin(y), _swPin(sw), _centerX(centerX), _centerY(centerY) {
}

void BJoystick::Setup() {
  pinMode(_swPin, INPUT_PULLUP);  
  _sw = digitalRead(_swPin);
  _x = analogRead(_xPin);
  _y = analogRead(_yPin);
  if (_centerX < 0) _centerX == _x;
  if (_centerY < 0) _centerY == _y;
}

void BJoystick::Update() {
  
  auto x = analogRead(_xPin);
  auto y = analogRead(_yPin);

  if (x > _centerX + _threshold || x < _centerX - _threshold || y > _centerY + _threshold || y < _centerY - _threshold) {
    BJoystickMoveArgs args;
    args.oldX = _x;
    args.oldY = _y;
    args.X = x;
    args.Y = y;
    _x = x;
    _y = y;
    OnMove(this, &args);
  }

  auto sw = _sw;
  _sw = digitalRead(_swPin);
  if (!_sw && sw) {
    OnClick(this, 0);
  }
}

#endif