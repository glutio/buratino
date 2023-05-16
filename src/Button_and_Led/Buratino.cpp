#include "Buratino.h"

Buratino Buratino::_instance;
BTaskSwitcher Buratino::_taskSwitcher;

Buratino::Buratino()
  : _initialized(false) {
}

int8_t Buratino::RunTask(BTask task, BTask::ArgumentType* arg, uint16_t stackSize) {
  if (!_instance._initialized) {
    return -1;
  }
  return _taskSwitcher.RunTask(task, arg, stackSize);
}

void Buratino::Setup(int8_t tasks) {
  if (!_instance._initialized) {
    _instance._initialized = true;
    _instance._taskSwitcher.Setup(tasks);
    _instance._taskSwitcher.Start();
  }
}
void Buratino::Start()
{
  if (_instance._initialized) {
    _instance._taskSwitcher.Start();
  }
}
void Buratino::YieldTask() {
  _taskSwitcher.YieldTask();
}

void Buratino::KillTask(int8_t id) {
  _taskSwitcher.KillTask(id);
}

int8_t Buratino::CurrentTask() {
  return _taskSwitcher.CurrentTask();
}