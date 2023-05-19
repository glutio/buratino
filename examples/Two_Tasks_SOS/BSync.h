#ifndef __BSYNC_H__
#define __BSYNC_H__

#include "BTaskSwitcher.h"

namespace B {
  bool disable();
  void restore(bool);
}

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
    auto sreg = B::disable();
    _value += rhs;
    B::restore(sreg);
    return *this;
  }

  // Overloading -= operator
  BSync& operator-=(const T& rhs) {
    auto sreg = B::disable();
    _value -= rhs;
    B::restore(sreg);
    return *this;
  }

  // Overloading *= operator
  BSync& operator*=(const T& rhs) {
    auto sreg = B::disable();
    _value *= rhs;
    B::restore(sreg);
    return *this;
  }

  // Overloading /= operator
  BSync& operator/=(const T& rhs) {
    auto sreg = B::disable();
    _value /= rhs;
    B::restore(sreg);
    return *this;
  }

  // Overloading = operator
  BSync& operator=(const T& rhs) {
    auto sreg = B::disable();
    if (&_value != &rhs) {
      _value = rhs;
    }
    B::restore(sreg);
    return *this;
  }

  // Overloading cast to T
  operator T() const {
    auto sreg = B::disable();
    T temp = _value;
    B::restore(sreg);
    return temp;
  }

  // Overloading == operator for T
  bool operator==(const T& rhs) const {
    auto sreg = B::disable();
    bool result = (_value == rhs);
    B::restore(sreg);
    return result;
  }

  // Overloading != operator for T
  bool operator!=(const T& rhs) const {
    auto sreg = B::disable();
    bool result = (_value != rhs);
    B::restore(sreg);
    return result;
  }


  // Overloading < operator for T
  bool operator<(const T& rhs) const {
    auto sreg = B::disable();
    bool result = (_value < rhs);
    B::restore(sreg);
    return result;
  }

  // Overloading > operator for T
  bool operator>(const T& rhs) const {
    auto sreg = B::disable();
    bool result = (_value > rhs);
    B::restore(sreg);
    return result;
  }

  T operator-() const {
    auto sreg = B::disable();
    T result = -_value;
    B::restore(sreg);
    return result;
  }

  // Logical negation
  bool operator!() const {
    auto sreg = B::disable();
    bool result = !_value;
    B::restore(sreg);
    return result;
  }

  // Increment (prefix)
  BSync& operator++() {
    auto sreg = B::disable();
    ++_value;
    B::restore(sreg);
    return *this;
  }

  // Decrement (prefix)
  BSync& operator--() {
    auto sreg = B::disable();
    --_value;
    B::restore(sreg);
    return *this;
  }

  // Increment (postfix)
  BSync operator++(int) {
    BSync temp(*this);
    auto sreg = B::disable();
    _value++;
    B::restore(sreg);
    return temp;
  }

  // Decrement (postfix)
  BSync operator--(int) {
    BSync temp(*this);
    auto sreg = B::disable();
    _value--;
    B::restore(sreg);
    return temp;
  }

  // Bitwise NOT
  BSync operator~() {
    auto sreg = B::disable();
    BSync temp(~_value);
    B::restore(sreg);
    return temp;
  }

  // Bitwise AND
  BSync operator&(const T& rhs) {
    auto sreg = B::disable();
    BSync temp(_value & rhs);
    B::restore(sreg);
    return temp;
  }

  // Bitwise OR
  BSync operator|(const T& rhs) {
    auto sreg = B::disable();
    BSync temp(_value | rhs);
    B::restore(sreg);
    return temp;
  }

  // Logical AND
  bool operator&&(const T& rhs) {
    auto sreg = B::disable();
    bool result = _value && rhs;
    B::restore(sreg);
    return result;
  }

  // Logical OR
  bool operator||(const T& rhs) {
    auto sreg = B::disable();
    bool result = _value || rhs;
    B::restore(sreg);
    return result;
  }

  // Bitwise AND assignment
  BSync& operator&=(const T& rhs) {
    auto sreg = B::disable();
    _value &= rhs;
    B::restore(sreg);
    return *this;
  }

  // Bitwise OR assignment
  BSync& operator|=(const T& rhs) {
    auto sreg = B::disable();
    _value |= rhs;
    B::restore(sreg);
    return *this;
  }

  // Left shift
  BSync operator<<(int shift) {
    auto sreg = B::disable();
    BSync temp(_value << shift);
    B::restore(sreg);
    return temp;
  }

  // Right shift
  BSync operator>>(int shift) {
    auto sreg = B::disable();
    BSync temp(_value >> shift);
    B::restore(sreg);
    return temp;
  }

  // Left shift assignment
  BSync& operator<<=(int shift) {
    auto sreg = B::disable();
    _value <<= shift;
    B::restore(sreg);
    return *this;
  }

  // Right shift assignment
  BSync& operator>>=(int shift) {
    auto sreg = B::disable();
    _value >>= shift;
    B::restore(sreg);
    return *this;
  }

  // Modulus and modulus assignment
  T operator%(const T& rhs) const {
    auto sreg = B::disable();
    T result = _value % rhs;
    B::restore(sreg);
    return result;
  }
  BSync& operator%=(const T& rhs) {
    auto sreg = B::disable();
    _value %= rhs;
    B::restore(sreg);
    return *this;
  }

  // Bitwise XOR and XOR assignment
  T operator^(const T& rhs) const {
    auto sreg = B::disable();
    T result = _value ^ rhs;
    B::restore(sreg);
    return result;
  }

  BSync& operator^=(const T& rhs) {
    auto sreg = B::disable();
    _value ^= rhs;
    B::restore(sreg);
    return *this;
  }

  // Array subscripting
  T& operator[](size_t index) {
    // Assume _value is an array or supports array-like access
    auto sreg = B::disable();
    T& result = _value[index];
    B::restore(sreg);
    return result;
  }
};

#endif