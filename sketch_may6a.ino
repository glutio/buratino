#include <LedControl.h>
#include "Heart.h"
#include "Buratino.h"

namespace HAL {
BDigitalPin button(9, INPUT_PULLUP, BPinTrigger::Change);
BDigitalPin motion(7, INPUT_PULLUP, BPinTrigger::Change);
BAnalogPin pot(0);
LedControl lc(12, 10, 11, 1);
BJoystick joystick(1, 2, 2, 518, 522);
BAnalogPin mic(3, BPinTrigger::Always);
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

    HAL::joystick.OnMove = BJoystick::MoveEvent(this, &App::OnJoystickMove);
    HAL::joystick.OnClick = BJoystick::ClickEvent(this, &App::OnJoystickClick);
    // HAL::button.OnChange = DigitalPin::ChangeEvent(this, &App::OnButtonClick);
    HAL::motion.OnChange = BDigitalPin::ChangeEvent(this, &App::OnMotionChange);
    // HAL::pot.OnChange = AnalogPin::ChangeEvent(this, &App::OnBrightnessChange);
    // HAL::mic.OnChange = AnalogPin::ChangeEvent(this, &App::OnMicChange);
    HAL::lc.shutdown(0, false);
  }

  void OnJoystickMove(BJoystick* joystick, BJoystickMoveArgs* args) {
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
    // noInterrupts();
    // Serial.println("task2");
    // Serial.flush();
    // interrupts();

    auto x = (int8_t)pixel % 8;
    auto y = (int8_t)pixel / 8;

    while (1) {
      noInterrupts();
      auto i = (_offset + y) % sizeof(heart);
      auto h = _shift < 0 ? heart[i] << -_shift : heart[i] >> _shift;
      auto state = h & (1 << x);
      HAL::lc.setLed(0, 8 - x - 1, 8 - y - 1, state);
      // Serial.println("task2");
      // Serial.flush();
      interrupts();
      delay(1);
      Buratino::YieldTask();
    }
  }

  void OnMicChange(BAnalogPin*, BAnalogPinChangeArgs* args) {
  }

  void OnJoystickClick(BJoystick* joystick, void*) {
    static auto task = BTask(this, &App::Draw);
    _scroll = !_scroll;
    if (!_scroll) {
      Buratino::KillTask(1);
    }
    else
    {
      Buratino::RunTask(task, 0, 56);    
    }
      //Serial.print(9);
  }

  void OnTimerTick(Buratino*, void*) {
    auto m = millis();
    auto n = millis();
    while (1) {
      m = millis();
      delay(1);
      n = millis() - m;
      noInterrupts();
      Serial.println(n);
      Serial.flush();
      interrupts();
      Buratino::YieldTask();
    }
  }

  void OnButtonClick(BDigitalPin* pin, BDigitalPinChangeArgs* args) {
    if (!args->value) {
      _scrollUp = !_scrollUp;
    }
  }

  void OnMotionChange(BDigitalPin* pin, BDigitalPinChangeArgs* args) {
    noInterrupts();
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
    interrupts();
  }

  void OnBrightnessChange(BAnalogPin* pin, BAnalogPinChangeArgs* args) {
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
  Buratino::Setup(64);
  noInterrupts();
  HAL::button.Setup();
  HAL::motion.Setup();
  HAL::pot.Setup();
  HAL::joystick.Setup();
  HAL::mic.Setup();

  auto task = BTask(&app, &App::Draw);
  for (auto i = 0; i < 64; ++i) {
    Buratino::RunTask(task, (void*)i, 56);
  }

  Serial.println();
  Serial.println("done");
  Serial.flush();
  pinMode(6, OUTPUT);
  //  digitalWrite(6, LOW);
  interrupts();
}

void loop() {
  HAL::button.Update();
  HAL::motion.Update();
  HAL::pot.Update();
  HAL::joystick.Update();
  HAL::mic.Update();
  // noInterrupts();
  Buratino::YieldTask();
  // interrupts();

  // noInterrupts();
  // Serial.println("task1");
  // Serial.flush();
  // interrupts();
  //delay(100);
}
