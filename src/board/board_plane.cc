/*
Copyright 2020 All rights reserved.
*/

#include "board/board_plane.h"

#include <torch/torch.h>
#include <cstring>
#include <iostream>
#include <tuple>

namespace aithena {

BoardPlane::BoardPlane(int width, int height)
    : width_{width},
      height_{height},
      plane_{/*num_bits=*/width * height, /*long value=*/0} {}

BoardPlane::BoardPlane(std::uint64_t plane)
    : width_{8}, height_{8}, plane_{64} {
  for (int i = 0; i < 64; i++) plane_[i] = (plane << i);
}

int BoardPlane::Count() { return plane_.count(); }

int BoardPlane::count() { return Count(); }

void BoardPlane::Set(int x, int y) {
  assert(x < width_ && y < height_);
  plane_.set(x + y * width_);
}

void BoardPlane::set(int x, int y) { return Set(x, y); }

void BoardPlane::Set(int x, int y, bool value) {
  assert(x < width_ && y < height_);

  if (value) return Set(x, y);

  return Clear(x, y);
}

// Clears the bit of the board at the specified location.
void BoardPlane::Clear(int x, int y) {
  assert(x < width_ && y < height_);
  plane_.reset(x + y * width_);
}

void BoardPlane::clear(int x, int y) { return Clear(x, y); }

// Returns the bit of the board at the specified location.
bool BoardPlane::Get(int x, int y) const {
  assert(x < width_ && y < height_);
  return plane_[x + y * width_];
}

bool BoardPlane::get(int x, int y) const { return Get(x, y); }

void BoardPlane::Rotate() {
  int x_, y_;
  bool bit;

  for (int y = 0; y < (height_ / 2); ++y) {
    y_ = height_ - y - 1;

    for (int x = 0; x < width_; ++x) {
      x_ = width_ - x - 1;

      bit = Get(x, y);

      Set(x, y, Get(x_, y_));
      Set(x_, y_, bit);
    }
  }
}

Coords BoardPlane::GetCoords() {
  Coords coords;

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      if (get(x, y) == 1) coords.push_back({x, y});
    }
  }

  return coords;
}

void BoardPlane::ScanLine(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx == 0 && dy == 0) {
    return;
  } else if (dx == 0) {
    // vertical
    dy = dy / abs(dy);
  } else if (dy == 0) {
    // horizontal
    dx = dx / abs(dx);
  } else if (abs(dx) == abs(dy)) {
    // diagonal
    dx = dx / abs(dx);
    dy = dy / abs(dy);
  } else {
    return;
  }

  for (int x = x1, y = y1; x != x2 || y != y2; x += dx, y += dy) set(x, y);
}  // namespace aithena

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
  BoardPlane result(*this);
  result.plane_ = ~plane_;

  return result;
}

bool BoardPlane::operator==(const BoardPlane& other) const {
  return plane_ == other.plane_;
}

bool BoardPlane::operator!=(const BoardPlane& other) const {
  return plane_ != other.plane_;
}

bool BoardPlane::IsEmpty() const { return plane_.none(); }

bool BoardPlane::empty() const { return IsEmpty(); }

std::vector<char> BoardPlane::ToBytes() {
  // Collect data
  struct BoardPlaneByteRepr board_struct;

  board_struct.width = width_;
  board_struct.height = height_;

  int byte_count = width_ * height_ / 8 + ((width_ * height_) % 2);
  unsigned char plane_data[byte_count];

  for (int i = 0; i < byte_count; ++i) plane_data[i] = '\0';

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

torch::Tensor BoardPlane::AsTensor() const {
  torch::Tensor res =
      torch::zeros({static_cast<int>(width_), static_cast<int>(height_)});
  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      if (plane_[y * width_ + x]) res.index_put_({x, y}, 1);
    }
  }
  return res;
}

}  // namespace aithena
