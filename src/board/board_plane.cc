/*
Copyright 2020 All rights reserved.
*/

#include "board/board_plane.h"

#include <cstring>
#include <iostream>

namespace aithena {

BoardPlane::BoardPlane(std::size_t width, std::size_t height)
    : width_{width},
      height_{height},
      plane_{/*num_bits=*/width * height, /*long value=*/0} {}

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

bool BoardPlane::empty() const { return plane_.none(); }

std::vector<char> BoardPlane::ToBytes() {
  // Collect data
  struct BoardPlaneByteRepr board_struct;

  board_struct.width = width_;
  board_struct.height = height_;

  int byte_count = width_ * height_ / 8 + ((width_ * height_) % 2);
  unsigned char plane_data[byte_count] = {0};

  for (int i = 0; i < static_cast<int>(width_); ++i) {
    for (int j = 0; j < static_cast<int>(height_); ++j) {
      if (get(i, j))
        plane_data[(i * height_ + j) / 8] |= (1 << ((i * height_ + j) % 8));
    }
  }

  // Write data to output buffer.
  std::vector<char> output(sizeof board_struct + byte_count);

  std::memcpy(&output[0], static_cast<char*>(static_cast<void*>(&board_struct)),
              sizeof board_struct);
  std::memcpy(&output[sizeof board_struct],
              static_cast<char*>(static_cast<void*>(&plane_data)), byte_count);

  return output;
}

std::tuple<BoardPlane, int> BoardPlane::FromBytes(std::vector<char> bytes) {
  int bytes_read = 0;

  struct BoardPlaneByteRepr* board_struct =
      static_cast<struct BoardPlaneByteRepr*>(static_cast<void*>(&bytes[0]));
  bytes_read += sizeof *board_struct;

  assert(board_struct->width < 255);
  assert(board_struct->height < 255);

  BoardPlane plane(board_struct->width, board_struct->height);

  for (int i = 0; i < board_struct->width; ++i) {
    for (int j = 0; j < board_struct->height; ++j) {
      if ((i * board_struct->height + j) % 8 == 0) ++bytes_read;

      bool value =
          (bytes[bytes_read - 1] >> ((i * board_struct->height + j) % 8)) & 1;

      if (value) plane.set(i, j);
    }
  }

  return std::make_tuple(plane, bytes_read);
}

}  // namespace aithena
