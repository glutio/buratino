#ifdef ARDUINO_ARCH_AVR

#define __BTASKSWITCHER_ARCH_HEADER__ \
  extern "C" void TIMER1_COMPA_vect();

#define __BTASKSWITCHER_ARCH_CLASS__ \
  static void switch_context(); \
  friend void TIMER1_COMPA_vect();

#endif