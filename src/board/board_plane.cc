/*
Copyright 2020 All rights reserved.
*/

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

// Sets the bit of the board at the specified location.
void BoardPlane::set(unsigned pos) {
  assert(pos < width_ * height_);
  plane_.set(pos);
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
  plane_ &= other.plane_;
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

BoardPlane BoardPlane::operator&(const BoardPlane& other) const {
  BoardPlane result(*this);
  result.plane_ = plane_ & other.plane_;

  return result;
}

BoardPlane BoardPlane::operator|(const BoardPlane& other) const {
  BoardPlane result(*this);
  result.plane_ = plane_ | other.plane_;

  return result;
}

BoardPlane BoardPlane::operator^(const BoardPlane& other) const {
  BoardPlane result(*this);
  result.plane_ = plane_ ^ other.plane_;

  return result;
}

BoardPlane BoardPlane::operator!() const {
  BoardPlane result{width_, height_};
  result.plane_ = ~plane_;

  return result;
}

bool BoardPlane::operator==(const BoardPlane& other) const {
  return plane_ == other.plane_;
}

bool BoardPlane::operator!=(const BoardPlane& other) const {
  return plane_ != other.plane_;
}

signed BoardPlane::find_first() const {
  return plane_.find_first();
}

signed BoardPlane::msb() const {
  signed index = -1;
  signed iter = plane_.find_first();
  while (iter >= 0) {
    index = iter;
    iter = plane_.find_next(iter);
  }
  return index;
}

signed BoardPlane::next(unsigned pos) const {
  return plane_.find_next(pos);
}

signed BoardPlane::prev(unsigned pos) const {
  signed index = -1;
  signed lookahead = plane_.find_first();
  while (lookahead >= 0 && static_cast<unsigned>(lookahead) < pos) {
    index = lookahead;
    lookahead = plane_.find_next(lookahead);
  }
  return index;
}

signed BoardPlane::count() const {
  return plane_.count();
}

void BoardPlane::reset() {
  plane_.reset();
}

bool BoardPlane::empty() const {
  return plane_.none();
}

}  // namespace aithena
