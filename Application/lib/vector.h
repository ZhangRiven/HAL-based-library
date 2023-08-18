/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-23 17:32:44
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-24 01:22:18
 * @FilePath     : \AD9954_HAL_F407ZGT6\Application\lib\vector.h
 * @Description  :
 */
#ifndef __VECTOR_H
#define __VECTOR_H
#include <cstring>
template <class T> class vector {
public:
  typedef T value_type;
  typedef T *iterator;
  vector() { _start = _end = _finish = nullptr; }
  ~vector() { delete[] _start; }
  vector& push_back(const T &e) {
    if (_end == _finish) {
      resize();
    }
    *_end = e;
    _end++;
    return *this;
  }
  vector& pop() {
    if (_start == _end) {
      return false;
    }
    _end--;
    return *this;
  }
  T back() { return *(_end - 1); }
  T front() { return *_start; }
  iterator begin() { return _start; }
  iterator end() { return _end; }
  size_t size() { return end() - begin(); }
  T &operator[](int n) {
    if (n < 0 || n >= size()) {
      //exit(0);//TODO:可能造成问题，注意检查
    }
    return *(_start + n);
  }

protected:
  iterator _start;
  iterator _end;
  iterator _finish;

private:
  void resize() {
    int old_size = _finish - _start;
    int new_size = old_size == 0 ? 1 : 2 * old_size;
    iterator new_start = new T[new_size];
    memcpy(new_start, _start, old_size * sizeof(T));
    delete[] _start;
    _start = new_start;
    _finish = _start + new_size;
    _end = _start + old_size;
  }
};

#endif