#include "board/board.h"

namespace aithena {

BoardPlane::BoardPlane(std::size_t width, std::size_t height)
  : width_{width}, height_{height}, plane_{width, height} {};

BoardPlane::BoardPlane(std::size_t width, std::size_t height, boost::dynamic_bitset<> plane)
  : width_{width}, height_{height}, plane_(plane) {};

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

boost::dynamic_bitset<>& BoardPlane::operator&=(const BoardPlane& b) {
  return plane_ &= b.plane_;
}

boost::dynamic_bitset<>& BoardPlane::operator|=(const BoardPlane& b) {
  return plane_ |= b.plane_;
}

boost::dynamic_bitset<>& BoardPlane::operator^=(const BoardPlane& b) {
  return plane_ ^= b.plane_;
}

boost::dynamic_bitset<>& BoardPlane::operator-=(const BoardPlane& b) {
  return plane_ -= b.plane_;
}

bool BoardPlane::operator == (const BoardPlane& b) {
  return plane_ == b.plane_;
}

bool BoardPlane::operator!=(const BoardPlane& b) {
  return plane_ != b.plane_;
}

bool BoardPlane::operator<(const BoardPlane& b) {
  return plane_ < b.plane_;
}

bool BoardPlane::operator<=(const BoardPlane& b) {
  return plane_ <= b.plane_;
}

bool BoardPlane::operator>(const BoardPlane& b) {
  return plane_ > b.plane_;
}

bool BoardPlane::operator>=(const BoardPlane& b) {
  return plane_ >= b.plane_;
}

boost::dynamic_bitset<> BoardPlane::operator&(const BoardPlane& b) {
  return plane_ & b.plane_;
}

boost::dynamic_bitset<> BoardPlane::operator|(const BoardPlane& b) {
  return plane_ | b.plane_;
}

boost::dynamic_bitset<> BoardPlane::operator^(const BoardPlane& b) {
  return plane_ ^ b.plane_;
}

boost::dynamic_bitset<> BoardPlane::operator-(const BoardPlane& b) {
  return plane_ - b.plane_;
}
}
