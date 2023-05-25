#ifndef __BSYNC_H__
#define __BSYNC_H__

#include "BTaskSwitcher.h"

template<typename T>
class BSync {
private:
  T _value;

public:
  BSync() {}

  BSync(const T& value)
    : _value(value) {}

  // Overloading += operator
  BSync& operator+=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value += rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Overloading -= operator
  BSync& operator-=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value -= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Overloading *= operator
  BSync& operator*=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value *= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Overloading /= operator
  BSync& operator/=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value /= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Overloading = operator
  BSync& operator=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    if (&_value != &rhs) {
      _value = rhs;
    }
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Overloading cast to T
  operator T() const {
    auto sreg = BTaskSwitcher::disable();
    T temp = _value;
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Overloading == operator for T
  bool operator==(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    bool result = (_value == rhs);
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Overloading != operator for T
  bool operator!=(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    bool result = (_value != rhs);
    BTaskSwitcher::restore(sreg);
    return result;
  }


  // Overloading < operator for T
  bool operator<(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    bool result = (_value < rhs);
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Overloading > operator for T
  bool operator>(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    bool result = (_value > rhs);
    BTaskSwitcher::restore(sreg);
    return result;
  }

  T operator-() const {
    auto sreg = BTaskSwitcher::disable();
    T result = -_value;
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Logical negation
  bool operator!() const {
    auto sreg = BTaskSwitcher::disable();
    bool result = !_value;
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Increment (prefix)
  BSync& operator++() {
    auto sreg = BTaskSwitcher::disable();
    ++_value;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Decrement (prefix)
  BSync& operator--() {
    auto sreg = BTaskSwitcher::disable();
    --_value;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Increment (postfix)
  BSync operator++(int) {
    BSync temp(*this);
    auto sreg = BTaskSwitcher::disable();
    _value++;
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Decrement (postfix)
  BSync operator--(int) {
    BSync temp(*this);
    auto sreg = BTaskSwitcher::disable();
    _value--;
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Bitwise NOT
  BSync operator~() {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(~_value);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Bitwise AND
  BSync operator&(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value & rhs);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Bitwise OR
  BSync operator|(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value | rhs);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Logical AND
  bool operator&&(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    bool result = _value && rhs;
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Logical OR
  bool operator||(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    bool result = _value || rhs;
    BTaskSwitcher::restore(sreg);
    return result;
  }

  // Bitwise AND assignment
  BSync& operator&=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value &= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Bitwise OR assignment
  BSync& operator|=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value |= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Left shift
  BSync operator<<(int shift) {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value << shift);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Right shift
  BSync operator>>(int shift) {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value >> shift);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  // Left shift assignment
  BSync& operator<<=(int shift) {
    auto sreg = BTaskSwitcher::disable();
    _value <<= shift;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Right shift assignment
  BSync& operator>>=(int shift) {
    auto sreg = BTaskSwitcher::disable();
    _value >>= shift;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Modulus and modulus assignment
  T operator%(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    T result = _value % rhs;
    BTaskSwitcher::restore(sreg);
    return result;
  }
  BSync& operator%=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value %= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Bitwise XOR and XOR assignment
  T operator^(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    T result = _value ^ rhs;
    BTaskSwitcher::restore(sreg);
    return result;
  }

  BSync& operator^=(const T& rhs) {
    auto sreg = BTaskSwitcher::disable();
    _value ^= rhs;
    BTaskSwitcher::restore(sreg);
    return *this;
  }

  // Array subscripting
  T& operator[](size_t index) {
    // Assume _value is an array or supports array-like access
    auto sreg = BTaskSwitcher::disable();
    T& result = _value[index];
    BTaskSwitcher::restore(sreg);
    return result;
  }


  BSync operator+(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value + rhs);
    BTaskSwitcher::restore(sreg);
    return temp;
  }

  BSync operator-(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value - rhs);
    BTaskSwitcher::restore(sreg);
    return temp;  
    }

  BSync operator*(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value * rhs);
    BTaskSwitcher::restore(sreg);
    return temp;  
  }

  BSync operator/(const T& rhs) const {
    auto sreg = BTaskSwitcher::disable();
    BSync temp(_value / rhs);
    BTaskSwitcher::restore(sreg);
    return temp;  
  }
};

#endif