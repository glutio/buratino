#ifndef __BURATINO_H__
#define __BURATINO_H__

#include <Arduino.h>

#include "Bind.h"
#include "TaskSwitcher.h"
#include "Pins.h"
#include "Joystick.h"

class Buratino {
protected:
  bool _initialized;
  IBuratinoDevice** _devices;
  int8_t _count;
  int8_t _capacity;
  static Buratino _instance;
  static TaskSwitcher _taskSwitcher;

public:
  static Buratino& Default() {
    return _instance;
  }

protected:
  Buratino(int8_t capacity)
    : _capacity(capacity), _count(0), _initialized(0) {
    _devices = new IBuratinoDevice*(_capacity);
  }

public:
  void AddDevice(IBuratinoDevice* device) {
    if (_count == _capacity) {
      _capacity += 5;
      IBuratinoDevice** devices = new IBuratinoDevice*[_capacity];

      for (int8_t i = 0; i < _count; ++i) {
        devices[i] = _devices[i];
      }

      delete _devices;
      _devices = devices;
    }

    _devices[_count++] = device;
    if (_initialized) {
      device->Setup();
    }
  }

  void RemoveDevice(IBuratinoDevice* device) {
    for (int8_t i = 0; i < _count; ++i) {
      if (_devices[i] == device) {
        _devices[i] = nullptr;
        break;
      }
    }
  }

  void Setup() {
    if (!_initialized) {
      for (int8_t i = 0; i < _count; ++i) {
        if (_devices[i]) {
          _devices[i]->Setup();
        }
      }
      _taskSwitcher.Setup();
      _initialized = true;
    }
  }

  void Update() {
    if (_initialized) {
      for (int8_t i = 0; i < _count; ++i) {
        if (_devices[i]) {
          _devices[i]->Update();
        }
      }
    }
  }

  friend class Task;
};

#endif