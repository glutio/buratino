#ifndef __HARDWARE_H__
#define __HARDWARE_H__

struct IBuratinoDevice {
  virtual void Setup() = 0;
  virtual void Update() = 0;
};

#endif