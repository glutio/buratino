#ifndef __BIND_H__
#define __BIND_H__

template<typename T>
class RefCountedPtr {
private:
  T* ptr;
  int* refCount;

public:
  // Constructor
  explicit RefCountedPtr(T* p = nullptr)
    : ptr(p), refCount(new int(1)) {}

  // Copy constructor
  RefCountedPtr(const RefCountedPtr& other)
    : ptr(other.ptr), refCount(other.refCount) {
    (*refCount)++;
  }

  // Assignment operator
  RefCountedPtr& operator=(const RefCountedPtr& other) {
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
  ~RefCountedPtr() {
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

template<typename TSender, typename TArgument>
class EventDelegate {
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

    void Call(TSender* sender, TArgument* argument) {
      (_instance->*_method)(sender, argument);
    }
  };

  struct CallableFunctionImpl : public Callable {
    void (*_method)(TSender*, TArgument*);

    CallableFunctionImpl(void (*method)(TSender*, TArgument*))
      : _method(method) {}

    void Call(TSender* sender, TArgument* argument) {
      (_method)(sender, argument);
    }
  };

protected:
  RefCountedPtr<Callable> _callable;  // smart pointer

public:
  typedef TSender Sender;
  typedef TArgument Argument;

  template<typename TClass>
  EventDelegate(const TClass* instance, void (TClass::*method)(TSender* sender, TArgument* argument))
    : _callable(new CallableMethodImpl<TClass>(instance, method)) {}

  EventDelegate(void (*function)(TSender* sender, TArgument* argument))
    : _callable(new CallableFunctionImpl(function)) {}

  EventDelegate()
    : _callable(0) {}

  ~EventDelegate() {
  }

  void operator()(TSender* sender, TArgument* argument) {
    if (_callable) {
      _callable->Call(sender, argument);
    }
  }
};

#endif