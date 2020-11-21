#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <bitset>
#include <vector>

#include "boost/dynamic_bitset.hpp"

namespace aithena {

class BoardPlane {
 public:
  BoardPlane(std::size_t width, std::size_t height);

  // Sets the bit of the board at the specified location.
  void set(unsigned int x, unsigned int y) {
    plane_.set(x + y * width_);
  }
  // Clears the bit of the board at the specified location.
  void clear(unsigned int x, unsigned int y) {
    plane_.reset(x + y * width_);
  }
  // Returns the bit of the board at the specified location.
  bool get(unsigned int x, unsigned int y) const {
    return plane_[x + y * width_];
  }

 private:
  const std::size_t width_, height_;
  // A 2D bit plane specified rows to columns.
  // Example:
  //  plane_[5 + 2 * width_] // returns the fifth element of the second row
  boost::dynamic_bitset<> plane_;
};

class Board {
 public:
  Board(std::size_t width, std::size_t height);
 private:
  std::vector<BoardPlane> planes_;
};

}

#endif // AITHENTA_BOARD_BOARD_H
