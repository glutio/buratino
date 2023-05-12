#ifndef __BURATINO_H__
#define __BURATINO_H__

#include <Arduino.h>

#include "List.h"
#include "Bind.h"
#include "TaskSwitcher.h"
#include "Pins.h"
#include "Device.h"
#include "Joystick.h"

#ifndef BURATINO_MIN_TASKS
#define BURATINO_MIN_TASKS 10
#endif

#ifndef BURATINO_MIN_TASKS
#define BURATINO_MIN_TASKS 10
#endif


class Buratino {
protected:
  bool _initialized;
  List<IBuratinoDevice*> _devices;
  static TaskSwitcher _taskSwitcher;
  static Buratino _instance;
protected:
  Buratino();
 
public:
  static void RunTask(Task::TaskDelegate& task, Task::TaskDelegate::Argument* arg, int16_t stackSize = 128);
  static void AddDevice(IBuratinoDevice* device);
  static void Setup();
  static void Update();
  
  friend class Task;
  friend void run_task(TaskInfo* taskInfo);
};

#endif