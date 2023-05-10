#include <stdint.h>
#ifndef __TIMER_H__
#define __TIMER_H__

#include "Bind.h"

struct TimerArgs
{
  uintptr_t pc;  
};

class Timer
{
public:
  typedef EventDelegate<Timer, TimerArgs> TickEvent;
  TickEvent OnTick;  
};

class TimerOne: public Timer
{
public:
  static TimerOne instance;

public:
  TimerOne();

  void Setup();
  void Tick(TimerArgs* args);

  friend void ISR_Wrapper();
};

#endif