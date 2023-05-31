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
    BDisableInterrupts cli;
    _value += rhs;
    return *this;
  }

  // Overloading -= operator
  BSync& operator-=(const T& rhs) {
    BDisableInterrupts cli;
    _value -= rhs;
    return *this;
  }

  // Overloading *= operator
  BSync& operator*=(const T& rhs) {
    BDisableInterrupts cli;
    _value *= rhs;
    return *this;
  }

  // Overloading /= operator
  BSync& operator/=(const T& rhs) {
    BDisableInterrupts cli;
    _value /= rhs;
    return *this;
  }

  BSync& operator=(const BSync& rhs) {
    BDisableInterrupts cli;
    if (this != &rhs) {
      _value = rhs._value;
    }
    return *this;
  }

  BSync& operator=(const T& rhs) {
    BDisableInterrupts cli;
    _value = rhs;
    return *this;
  }

  // Overloading cast to T
  operator T() const {
    BDisableInterrupts cli;
    T temp = _value;
    return temp;
  }

  // Overloading == operator for T
  bool operator==(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value == rhs);
    return result;
  }

  // Overloading != operator for T
  bool operator!=(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value != rhs);
    return result;
  }

  // Overloading < operator for T
  bool operator<(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value < rhs);
    return result;
  }

  // Overloading > operator for T
  bool operator>(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value > rhs);
    return result;
  }

  // Overloading >= operator for T
  bool operator>=(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value >= rhs);
    return result;
  }

  // Overloading <= operator for T
  bool operator<=(const T& rhs) const {
    BDisableInterrupts cli;
    bool result = (_value <= rhs);
    return result;
  }

  T operator-() const {
    BDisableInterrupts cli;
    T result = -_value;
    return result;
  }

  // Logical negation
  bool operator!() const {
    BDisableInterrupts cli;
    bool result = !_value;
    return result;
  }

  // Increment (prefix)
  BSync& operator++() {
    BDisableInterrupts cli;
    ++_value;
    return *this;
  }

  // Decrement (prefix)
  BSync& operator--() {
    BDisableInterrupts cli;
    --_value;
    return *this;
  }

  // Increment (postfix)
  BSync operator++(int) {
    BDisableInterrupts cli;
    BSync temp(*this);
    ++_value;
    return temp;
  }

  // Decrement (postfix)
  BSync operator--(int) {
    BDisableInterrupts cli;
    BSync temp(*this);
    --_value;
    return temp;
  }

  // Bitwise NOT
  BSync operator~() {
    BDisableInterrupts cli;
    BSync temp(~_value);
    return temp;
  }

  // Bitwise AND
  BSync operator&(const T& rhs) {
    BDisableInterrupts cli;
    BSync temp(_value & rhs);
    return temp;
  }

  // Bitwise OR
  BSync operator|(const T& rhs) {
    BDisableInterrupts cli;
    BSync temp(_value | rhs);
    return temp;
  }

  // Logical AND
  bool operator&&(const T& rhs) {
    BDisableInterrupts cli;
    bool result = _value && rhs;
    return result;
  }

  // Logical OR
  bool operator||(const T& rhs) {
    BDisableInterrupts cli;
    bool result = _value || rhs;
    return result;
  }

  // Bitwise AND assignment
  BSync& operator&=(const T& rhs) {
    BDisableInterrupts cli;
    _value &= rhs;
    return *this;
  }

  // Bitwise OR assignment
  BSync& operator|=(const T& rhs) {
    BDisableInterrupts cli;
    _value |= rhs;
    return *this;
  }

  // Left shift
  BSync operator<<(int shift) {
    BDisableInterrupts cli;
    BSync temp(_value << shift);
    return temp;
  }

  // Right shift
  BSync operator>>(int shift) {
    BDisableInterrupts cli;
    BSync temp(_value >> shift);
    return temp;
  }

  // Left shift assignment
  BSync& operator<<=(int shift) {
    BDisableInterrupts cli;
    _value <<= shift;
    return *this;
  }

  // Right shift assignment
  BSync& operator>>=(int shift) {
    BDisableInterrupts cli;
    _value >>= shift;
    return *this;
  }

  // Modulus and modulus assignment
  T operator%(const T& rhs) const {
    BDisableInterrupts cli;
    T result = _value % rhs;
    return result;
  }
  BSync& operator%=(const T& rhs) {
    BDisableInterrupts cli;
    _value %= rhs;
    return *this;
  }

  // Bitwise XOR and XOR assignment
  T operator^(const T& rhs) const {
    BDisableInterrupts cli;
    T result = _value ^ rhs;
    return result;
  }

  BSync& operator^=(const T& rhs) {
    BDisableInterrupts cli;
    _value ^= rhs;
    return *this;
  }

  // // Array subscripting
  // T& operator[](size_t index) {
  //   // Assume _value is an array or supports array-like access
  //   BDisableInterrupts cli;
  //   T& result = _value[index];
  //   BTaskSwitcher::restore(sreg);
  //   return result;
  // }

  BSync operator+(const T& rhs) const {
    BDisableInterrupts cli;
    BSync temp(_value + rhs);
    return temp;
  }

  BSync operator-(const T& rhs) const {
    BDisableInterrupts cli;
    BSync temp(_value - rhs);
    return temp;
  }

  BSync operator*(const T& rhs) const {
    BDisableInterrupts cli;
    BSync temp(_value * rhs);
    return temp;
  }

  BSync operator/(const T& rhs) const {
    BDisableInterrupts cli;
    BSync temp(_value / rhs);
    return temp;
  }
};

#endif