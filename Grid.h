#ifndef RAINFALL_GRID_H
#define RAINFALL_GRID_H

#include <iostream>

template<typename T>
class Grid {
private:
  T *grid;
  int size;
public:
  explicit Grid(int n) {
    size = n;
    grid = new T[n * n];
  }

  ~Grid() {
    delete[] grid;
  }

  T *operator[](int x) {
    return (grid + x * size);
  }

  T *operator[](int x) const {
    return (grid + x * size);
  }
};

#endif //RAINFALL_GRID_H
