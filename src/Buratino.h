#ifndef __BURATINO_H__
#define __BURATINO_H__

#include <Arduino.h>

class Buratino;

#include "BList.h"
#include "BEvent.h"
#include "BSync.h"
#include "BTaskSwitcher.h"
#include "BPins.h"
#include "BJoystick.h"

class Buratino {
protected:
  bool _initialized;
  BList<uint8_t> _cli;
  static BTaskSwitcher _taskSwitcher;
  static Buratino _instance;
protected:
  Buratino();

public:
  static int8_t RunTask(BTask task, BTask::ArgumentType* arg, uint16_t stackSize = 128);
  static void Setup(int8_t tasks);
  static void YieldTask();
  static void KillTask(int8_t id);
  static void Start();
  static void Stop();
  static int8_t CurrentTask();
};

#endif