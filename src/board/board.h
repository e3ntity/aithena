#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <array>
#include <bitset>

namespace aithena {

template <std::size_t Width, std::size_t Height>
class BoardPlane {
 public:
  BoardPlane() : plane_() {};

  // Sets the bit of the board at the specified location.
  void set(unsigned int x, unsigned int y) {
    plane_[x + y * Width] = true;
  }
  // Clears the bit of the board at the specified location.
  void clear(unsigned int x, unsigned int y) {
    plane_[x + y * Width] = false;
  }
  // Returns the bit of the board at the specified location.
  bool get(unsigned int x, unsigned int y) const {
    return plane_[x + y * Width];
  }

 private:
  // A 2D bit plane specified rows to columns.
  // Example:
  //  plane_[5 + 2 * Width] // returns the fifth element of the second row
  std::bitset<Width * Height> plane_;
};

class Board {
 public:
  Board(std::size_t width, std::size_t height); // TODO: include figure information.
};

}

#endif // AITHENTA_BOARD_BOARD_H
