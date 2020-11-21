#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <bitset>
#include <vector>

#include "boost/dynamic_bitset.hpp"

namespace aithena {

class BoardPlane {
public:
  BoardPlane(std::size_t width, std::size_t height);
  BoardPlane(std::size_t width, std::size_t height, boost::dynamic_bitset<> plane);

  // Sets the bit of the board at the specified location.
  void set(unsigned int x, unsigned int y);
  // Clears the bit of the board at the specified location.
  void clear(unsigned int x, unsigned int y);
  // Returns the bit of the board at the specified location.
  bool get(unsigned int x, unsigned int y) const;

  boost::dynamic_bitset<>& operator&=(const BoardPlane& b);
  boost::dynamic_bitset<>& operator|=(const BoardPlane& b);
  boost::dynamic_bitset<>& operator^=(const BoardPlane& b);
  boost::dynamic_bitset<>& operator-=(const BoardPlane& b);
  bool operator==(const BoardPlane& b);
  bool operator!=(const BoardPlane& b);
  bool operator<(const BoardPlane& b);
  bool operator<=(const BoardPlane& b);
  bool operator>(const BoardPlane& b);
  bool operator>=(const BoardPlane& b);
  boost::dynamic_bitset<> operator&(const BoardPlane& b);
  boost::dynamic_bitset<> operator|(const BoardPlane& b);
  boost::dynamic_bitset<> operator^(const BoardPlane& b);
  boost::dynamic_bitset<> operator-(const BoardPlane& b);


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
