/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_BOARD_BOARD_PLANE_H_
#define SRC_BOARD_BOARD_PLANE_H_

#include <vector>

#include "boost/dynamic_bitset.hpp"

namespace aithena {

class BoardPlane {
 public:
  BoardPlane(std::size_t width, std::size_t height);

  // Special constructor for chess.
  explicit BoardPlane(std::uint64_t);
  BoardPlane() = default;

  // Sets the bit of the board at the specified location.
  void set(unsigned int x, unsigned int y);
  // Sets the bit of the board at the specified location.
  void set(unsigned pos);
  // Clears the bit of the board at the specified location.
  void clear(unsigned int x, unsigned int y);
  // Returns the bit of the board at the specified location.
  bool get(unsigned int x, unsigned int y) const;

  BoardPlane& operator=(const BoardPlane&);

  BoardPlane& operator&=(const BoardPlane&);
  BoardPlane& operator|=(const BoardPlane&);
  BoardPlane& operator^=(const BoardPlane&);

  BoardPlane operator&(const BoardPlane&) const;
  BoardPlane operator|(const BoardPlane&) const;
  BoardPlane operator^(const BoardPlane&) const;
  BoardPlane operator!() const;

  bool operator==(const BoardPlane&) const;
  bool operator!=(const BoardPlane&) const;

  signed find_first() const;
  signed msb() const;
  signed next(unsigned pos) const;
  signed prev(unsigned pos) const;
  signed count() const;
  void reset();

  // Returns true if no bits are set, otherwise false.
  bool empty() const;

 private:
  std::size_t width_, height_;
  // A 2D bit plane specified rows to columns.
  // Example:
  //  plane_[5 + 2 * width_] // returns the fifth element of the second row
  boost::dynamic_bitset<> plane_;
};

}  // namespace aithena

#endif  // SRC_BOARD_BOARD_PLANE_H_
