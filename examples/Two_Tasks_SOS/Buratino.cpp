#include "Buratino.h"

Buratino Buratino::_instance;
BTaskSwitcher Buratino::_taskSwitcher;

Buratino::Buratino()
  : _initialized(false), _cli(3) {
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

void Buratino::Stop() {
  _instance._cli.Add(B::disable());
}

void Buratino::Start() {
  if (!_instance._cli.Length()) {
    Stop();
  }

  auto sreg = _instance._cli[_instance._cli.Length() - 1];
  B::restore(sreg);
  _instance._cli.Remove(_instance._cli.Length() - 1);
}