#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "BEvent.h"

class BJoystick;
class BJoystickMoveArgs;

struct BJoystickMoveArgs {
  uint16_t oldX;
  uint16_t oldY;
  uint16_t X;
  uint16_t Y;
};

class BJoystick {
public:
  typedef BEvent<BJoystick, BJoystickMoveArgs> MoveEvent;
  typedef BEvent<BJoystick, void> ClickEvent;

protected:
  const uint8_t _threshold = 25;

  uint8_t _xPin;
  uint8_t _yPin;
  uint8_t _swPin;

  int16_t _centerX;
  int16_t _centerY;

  uint16_t _x;
  uint16_t _y;
  bool _sw;

public:
  MoveEvent OnMove;
  ClickEvent OnClick;

public:
  BJoystick(uint8_t x, uint8_t y, uint8_t sw, int16_t centerX = -1, int16_t centerY = -1);
  void Calibrate();
  void Update();
};

#endif