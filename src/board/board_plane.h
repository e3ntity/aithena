/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_BOARD_BOARD_PLANE_H_
#define SRC_BOARD_BOARD_PLANE_H_

#include <torch/torch.h>
#include <vector>

#include "boost/dynamic_bitset.hpp"

namespace aithena {

struct Coord {
  int x;
  int y;
};

using Coords = std::vector<Coord>;

class BoardPlane {
 public:
  BoardPlane(int width, int height);

  // Special constructor for chess.
  explicit BoardPlane(std::uint64_t);
  BoardPlane() = default;

  // Returns the number of set bits.
  int count();
  // Sets the bit of the board at the specified location.
  void set(int x, int y);
  // Clears the bit of the board at the specified location.
  void clear(int x, int y);
  // Returns the bit of the board at the specified location.
  bool get(int x, int y) const;

  // Returns the coordinates (x, y) of all set bits.
  Coords GetCoords();

  // Sets all bits in the line connecting (x1, y1) and (x2, y2).
  // If there exists no direct vertical / horizontal / diagonal line, the
  // board remains unmodified.
  void ScanLine(int x1, int y1, int x2, int y2);

  BoardPlane& operator=(const BoardPlane&);

  BoardPlane& operator&=(const BoardPlane&);
  BoardPlane& operator|=(const BoardPlane&);
  BoardPlane& operator^=(const BoardPlane&);

  BoardPlane operator&(const BoardPlane&)const;
  BoardPlane operator|(const BoardPlane&) const;
  BoardPlane operator^(const BoardPlane&) const;
  BoardPlane operator!() const;

  bool operator==(const BoardPlane&) const;
  bool operator!=(const BoardPlane&) const;

  // Returns true if no bits are set, otherwise false.
  bool empty() const;

  // Returns a tensor representation of the board plane.
  torch::Tensor AsTensor() const;

  // Converts the board to bytes.
  std::vector<char> ToBytes();
  // Reads a byte representation of the board into a board object and returns
  // the number of bytes read.
  static std::tuple<BoardPlane, int> FromBytes(std::vector<char>);

 private:
  int width_, height_;
  // A 2D bit plane specified rows to columns.
  // Example:
  //  plane_[5 + 2 * width_] // returns the fifth element of the second row
  boost::dynamic_bitset<> plane_;

  struct BoardPlaneByteRepr {
    int width, height;
  };
};

}  // namespace aithena

#endif  // SRC_BOARD_BOARD_PLANE_H_
