#ifndef __BPTR_H__
#define __BPTR_H_

/*
  BPtr - a smart pointer implementation
*/
template<typename T>
class BPtr {
private:
  T* ptr;
  int* refCount;

public:
  // Constructor
  explicit BPtr(T* p = nullptr)
    : ptr(p), refCount(new int(1)) {
    Serial.print("ctor");
    Serial.print(" ");
    Serial.println(*refCount);
  }

  // Copy constructor
  BPtr(const BPtr& other)
    : ptr(other.ptr), refCount(other.refCount) {
    (*refCount)++;
    Serial.print("copy ctor");
    Serial.print(" ");
    Serial.println(*refCount);
  }

  // Assignment operator
  BPtr& operator=(const BPtr& other) {
    if (this != &other) {
      // Decrease the reference count of the current object
      (*refCount)--;

      // If the reference count becomes zero, delete the object and refCount
      if (*refCount == 0) {
        delete ptr;
        delete refCount;
      }

      // Assign the new object and increase its reference count
      ptr = other.ptr;
      refCount = other.refCount;
      (*refCount)++;
      Serial.print("assignment");
      Serial.print(" ");
      Serial.println(*refCount);
    }
    return *this;
  }

  // Destructor
  ~BPtr() {
    (*refCount)--;
    Serial.print("destructor");
    Serial.print(" ");
    Serial.println(*refCount);

    if (*refCount == 0) {
      delete ptr;
      delete refCount;
    }
  }

  // Dereference operator
  T& operator*() const {
    return *ptr;
  }

  // Arrow operator
  T* operator->() const {
    return ptr;
  }

  // Conversion operator to bool
  explicit operator bool() const {
    return ptr != nullptr;
  }
};
#endif