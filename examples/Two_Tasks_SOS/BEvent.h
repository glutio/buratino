#ifndef __BEVENT_H__
#define __BEVENT_H__

/*
  BRefCountedPtr - a smart pointer implementation
*/
template<typename T>
class BRefCountedPtr {
private:
  T* ptr;
  int* refCount;

public:
  // Constructor
  explicit BRefCountedPtr(T* p = nullptr)
    : ptr(p), refCount(new int(1)) {
  }

  // Copy constructor
  BRefCountedPtr(const BRefCountedPtr& other)
    : ptr(other.ptr), refCount(other.refCount) {
    (*refCount)++;
  }

  // Assignment operator
  BRefCountedPtr& operator=(const BRefCountedPtr& other) {
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
    }
    return *this;
  }

  // Destructor
  ~BRefCountedPtr() {
    (*refCount)--;
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


/*
  BEvent - a function/method delegate that takes two arguments
*/
template<typename TSender, typename TArgument>
class BEvent {
private:
  struct Callable {
    virtual ~Callable() {}
    virtual void Call(TSender* sender, TArgument* argument) = 0;
  };

  template<typename TClass>
  struct CallableMethodImpl : public Callable {
    TClass* _instance;
    void (TClass::*_method)(TSender*, TArgument*);

    CallableMethodImpl(TClass* instance, void (TClass::*method)(TSender*, TArgument*))
      : _instance(instance), _method(method) {}

    virtual void Call(TSender* sender, TArgument* argument) {
      (_instance->*_method)(sender, argument);
    }
  };

  struct CallableFunctionImpl : public Callable {
    void (*_func)(TSender*, TArgument*);

    CallableFunctionImpl(void (*func)(TSender*, TArgument*))
      : _func(func) {}

    virtual void Call(TSender* sender, TArgument* argument) {
      (_func)(sender, argument);
    }
  };

protected:
  BRefCountedPtr<Callable> _callable;  // smart pointer

public:
  typedef TSender SenderType;
  typedef TArgument ArgumentType;

  template<typename TClass>
  BEvent(const TClass* instance, void (TClass::*method)(TSender* sender, TArgument* argument))
    : _callable(new CallableMethodImpl<TClass>(instance, method)) {}

  BEvent(void (*func)(TSender* sender, TArgument* argument))
    : _callable(new CallableFunctionImpl(func)) {}

  BEvent()
    : _callable(0) {}

  ~BEvent() {
  }

  void operator()(TSender* sender, TArgument* argument) {
    if (_callable) {
      _callable->Call(sender, argument);
    }
  }
};

#endif