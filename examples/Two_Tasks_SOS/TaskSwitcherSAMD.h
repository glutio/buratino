#ifdef ARDUINO_ARCH_SAMD

#define __BTASKSWITCHER_ARCH_HEADER__ \
  extern "C" int sysTickHook();

#define __BTASKSWITCHER_ARCH_CLASS__ \
  static void schedule_task(); \
  static uint8_t* switch_task(uint8_t* sp); \
  friend void PendSV_Handler(); \
  friend int sysTickHook();

#endif