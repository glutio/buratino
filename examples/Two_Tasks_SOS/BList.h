#ifndef __BLIST_H__
#define __BLIST_H__

template<typename T>
class BList {
protected:
  T* _array;
  uint16_t _count;
  uint16_t _capacity;

public:
  BList(uint16_t capacity)
    : _capacity(capacity), _count(0), _array(new T[capacity]) {}

  BList()
    : _capacity(0), _count(0), _array(0) {}

  ~BList() {
    delete[] _array;
  }

  T& operator[](uint16_t index) {
    return _array[index];
  }

  void Add(T item) {
    if (_count == _capacity) {
      if (!_capacity) {
        _capacity = 1;
      }
      Resize(_capacity * 2);
    }

    _array[_count++] = item;
  }

  void Remove(uint16_t index) {
    for (auto i = index; i < _count - 1; ++i) {
      _array[i] = _array[i + 1];
    }
    --_count;
  }
  
  uint16_t Length() {
    return _count;
  }

  uint16_t Capacity() {
    return _capacity;
  }

  void Resize(uint16_t capacity) {
    T* array = new T[capacity];
    for (auto i = 0; i < _capacity; ++i) {
      array[i] = _array[i];
    }
    delete[] _array;
    _array = array;
    _capacity = capacity;
  }
};


#endif