#include "board/board_plane.h"

namespace aithena {

BoardPlane::BoardPlane(std::size_t width, std::size_t height)
  : width_{width},
    height_{height},
    plane_{/*num_bits=*/ width * height, /*long value=*/ 0} {};

// Sets the bit of the board at the specified location.
void BoardPlane::set(unsigned int x, unsigned int y) {
  plane_.set(x + y * width_);
}
// Clears the bit of the board at the specified location.
void BoardPlane::clear(unsigned int x, unsigned int y) {
  plane_.reset(x + y * width_);
}
// Returns the bit of the board at the specified location.
bool BoardPlane::get(unsigned int x, unsigned int y) const {
  return plane_[x + y * width_];
}

// Overloaded operators go here

BoardPlane& BoardPlane::operator=(const BoardPlane& other) {
  if (&other == this) return *this;

  width_ = other.width_;
  height_ = other.height_;
  plane_ = other.plane_;

  return *this;
}

BoardPlane& BoardPlane::operator&=(const BoardPlane& other) {
  plane_ |= other.plane_;
  return *this;
}

BoardPlane& BoardPlane::operator|=(const BoardPlane& other) {
  plane_ |= other.plane_;
  return *this;
}

BoardPlane& BoardPlane::operator^=(const BoardPlane& other) {
  plane_ ^= other.plane_;
  return *this;
}

BoardPlane BoardPlane::operator&(const BoardPlane& other) {
  BoardPlane result(*this);
  result.plane_ = plane_ & other.plane_;

  return result;
}

BoardPlane BoardPlane::operator|(const BoardPlane& other) {
  BoardPlane result(*this);
  result.plane_ = plane_ | other.plane_;

  return result;
}

BoardPlane BoardPlane::operator^(const BoardPlane& other) {
  BoardPlane result(*this);
  result.plane_ = plane_ ^ other.plane_;

  return result;
}

bool BoardPlane::operator==(const BoardPlane& other) {
  return plane_ == other.plane_;
}

bool BoardPlane::operator!=(const BoardPlane& other) {
  return plane_ != other.plane_;
}

}
