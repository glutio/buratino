#include "Buratino.h"

Buratino Buratino::_instance;
TaskSwitcher Buratino::_taskSwitcher;

Buratino::Buratino()
  : _initialized(0), _devices(10) {
}

void Buratino::AddDevice(IBuratinoDevice* device) {
  _instance._devices.Add(device);
  if (_instance._initialized) {
    device->Setup();
  }
}

void Buratino::RunTask(BTask task, BTask::Argument* arg, int16_t stackSize) {
  _taskSwitcher.RunTask(task, arg, stackSize);
}

void Buratino::Setup() {
  if (!_instance._initialized) {
    for (int8_t i = 0; i < _instance._devices.Length(); ++i) {
      if (_instance._devices[i]) {
        _instance._devices[i]->Setup();
      }
    }
    _instance._taskSwitcher.Setup();
    _instance._initialized = true;
  }
}

void Buratino::Update() {
  if (_instance._initialized) {
    for (int8_t i = 0; i < _instance._devices.Length(); ++i) {
      if (_instance._devices[i]) {
        _instance._devices[i]->Update();
      }
    }

    _taskSwitcher.Cleanup();
  }
}

void Buratino::YieldTask()
{
  _taskSwitcher.YieldTask();
}

void Buratino::KillTask(uint8_t id)
{
  _taskSwitcher.KillTask(id);  
}