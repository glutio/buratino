
#include <LedControl.h>
#include "Heart.h"
#include "Buratino.h"

namespace HAL {
DigitalPin button(9, INPUT_PULLUP, PinTrigger::Change);
DigitalPin motion(7, INPUT_PULLUP, PinTrigger::Change);
AnalogPin pot(0);
LedControl lc(12, 10, 11, 1);
Joystick joystick(1, 2, 2);
AnalogPin mic(3, PinTrigger::Always);
}

class App {
private:
  bool _scrollUp;
  bool _scroll;
  int8_t _offset;
  int8_t _shift;

public:
  void DrawHeart() {
    for (auto j = 0; j < 8; j++) {
      auto x = (_offset + j) % sizeof(heart);
      auto h = _shift < 0 ? heart[x] << -_shift : heart[x] >> _shift;
      HAL::lc.setColumn(0, 8 - j - 1, h);
    }
  }
  void Scroll() {
    if (_scrollUp) {
      if (++_offset == sizeof(heart)) _offset = 0;
    } else {
      if (--_offset < 0) _offset = sizeof(heart) - 1;
    }
  }

  App()
    : _scrollUp(true),
      _scroll(true) {

    HAL::joystick.OnMove = Joystick::MoveEvent(this, &App::OnJoystickMove);
    HAL::joystick.OnClick = Joystick::ClickEvent(this, &App::OnJoystickClick);
    // HAL::button.OnChange = DigitalPin::ChangeEvent(this, &App::OnButtonClick);
    HAL::motion.OnChange = DigitalPin::ChangeEvent(this, &App::OnMotionChange);
    // HAL::pot.OnChange = AnalogPin::ChangeEvent(this, &App::OnBrightnessChange);
    // HAL::mic.OnChange = AnalogPin::ChangeEvent(this, &App::OnMicChange);

  }

  void OnJoystickMove(Joystick* joystick, JoystickMoveArgs* args) {
    // if (_scroll) {
    //   return;
    // }

    _scrollUp = args->Y > 512;

    if (args->X < 512) {
      if (--_shift < -7) _shift = -7;
    } else {
      if (++_shift > 7) _shift = 7;
    }

    Scroll();
    //DrawHeart();
  }

  void Draw(Buratino*, void* pixel) {
    auto x = (int8_t)pixel % 8;
    auto y = (int8_t)pixel / 8;

    while (1) {
      auto i = (_offset + y) % sizeof(heart);
      auto h = _shift < 0 ? heart[x] << -_shift : heart[x] >> _shift;
      auto state = h & (1 << x);

      HAL::lc.setLed(0, x, y, state);

      delay(100);
    }
  }

  void OnMicChange(AnalogPin*, AnalogPinChangeArgs* args) {
  }

  void OnJoystickClick(Joystick* joystick, void*) {
    _scroll = !_scroll;
  }

  void OnButtonClick(DigitalPin* pin, DigitalPinChangeArgs* args) {
    if (!args->value) {
      _scrollUp = !_scrollUp;
    }
  }

  void OnMotionChange(DigitalPin* pin, DigitalPinChangeArgs* args) {
    if (args->value) {
      _offset = 0;
      if (!_scroll) {
        DrawHeart();
      }
      HAL::lc.shutdown(0, false);
    } else {
      HAL::lc.clearDisplay(0);
      HAL::lc.shutdown(0, true);
    }
  }

  void OnBrightnessChange(AnalogPin* pin, AnalogPinChangeArgs* args) {
    HAL::lc.setIntensity(0, map(args->value, 0, 1023, 0, 15));
  }
public:
  // void OnTimer(Timer* sender, TimerArgs* args) {
  //   if (HAL::motion.get_Value() && _scroll) {
  //     Scroll();
  //     DrawHeart();
  //   }
  // }
};

App app;

void setup() {
  Serial.begin(115200);


  Buratino::AddDevice(&HAL::button);
  Buratino::AddDevice(&HAL::motion);
  Buratino::AddDevice(&HAL::pot);
  Buratino::AddDevice(&HAL::joystick);
  Buratino::AddDevice(&HAL::mic);

  for (auto i = 0; i < 64; ++i) {
    Buratino::RunTask(BTask(&app, &App::Draw), (void*)i, 16);
  }

  Serial.println("done");
  Serial.flush();

  Buratino::Setup();
}

void loop() {
  Buratino::Update();
  Serial.println("loop");
}
