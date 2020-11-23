#include "board/board_plane.h"

namespace aithena {

BoardPlane::BoardPlane(std::size_t width, std::size_t height)
  : width_{width},
    height_{height},
    plane_{/*num_bits=*/ width * height, /*long value=*/ 0} {}

BoardPlane::BoardPlane(std::uint64_t plane)
  : width_{8}, height_{8}, plane_{64} {
  for (int i = 0; i < 64; i++) plane_[i] = (plane << i);
}

// Sets the bit of the board at the specified location.
void BoardPlane::set(unsigned x, unsigned y) {
  assert(x < width_ && y < height_);
  plane_.set(x + y * width_);
}

// Clears the bit of the board at the specified location.
void BoardPlane::clear(unsigned x, unsigned y) {
  assert(x < width_ && y < height_);
  plane_.reset(x + y * width_);
}

// Returns the bit of the board at the specified location.
bool BoardPlane::get(unsigned x, unsigned y) const {
  assert(x < width_ && y < height_);
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

BoardPlane BoardPlane::operator!() {
  BoardPlane result{width_, height_};
  result.plane_ = ~plane_;

  return result;
}

bool BoardPlane::operator==(const BoardPlane& other) {
  return plane_ == other.plane_;
}

bool BoardPlane::operator!=(const BoardPlane& other) {
  return plane_ != other.plane_;
}

}
