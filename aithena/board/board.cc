/*
Copyright 2020 All rights reserved.
*/

#include "board/board.h"

#include <torch/torch.h>
#include <cstring>
#include <iostream>

namespace aithena {

BoardPlane GetNewFields(const Board& before, const Board& after) {
  assert(before.width_ == after.width_ && before.height_ == after.height_ &&
         before.figure_count_ == after.figure_count_);

  BoardPlane difference(before.width_, before.height_);

  int plane_count = static_cast<int>(before.planes_.size());

  for (int plane = 0; plane < plane_count; ++plane)
    difference |= ((!before.planes_[plane]) & after.planes_[plane]);

  return difference;
}

// Board class member functions go here

bool operator==(const Piece& a, const Piece& b) {
  return a.figure == b.figure && a.player == b.player;
}

Board::Board(int width, int height, int figure_count)
    : width_(width),
      height_(height),
      figure_count_(figure_count),
      planes_(figure_count * 2, BoardPlane(width, height)) {}
Board::Board(const Board& other) { *this = other; }

Board& Board::operator=(const Board& other) {
  if (&other == this) return *this;

  planes_ = other.planes_;
  width_ = other.width_;
  height_ = other.height_;
  figure_count_ = other.figure_count_;

  return *this;
}

bool Board::operator==(const Board& other) const {
  return width_ == other.width_ && height_ == other.height_ &&
         figure_count_ == other.figure_count_ && planes_ == other.planes_;
}

bool Board::operator!=(const Board& other) const { return !(*this == other); }

int Board::GetWidth() { return width_; }
int Board::GetHeight() { return height_; }
int Board::GetFigureCount() { return figure_count_; }

BoardPlane& Board::GetPlane(Piece piece) {
  assert(piece.figure < figure_count_ && piece.player < 2);
  return planes_[piece.player * figure_count_ + piece.figure];
}

BoardPlane Board::GetFigurePlane(int figure) {
  return planes_[figure] | planes_[figure_count_ + figure];
}

BoardPlane Board::GetPlayerPlane(int player) {
  BoardPlane plane{width_, height_};

  for (int i = player * figure_count_; i < (player + 1) * figure_count_; ++i)
    plane |= planes_[i];

  return plane;
}

BoardPlane Board::GetCompletePlane() {
  BoardPlane plane{width_, height_};

  for (auto plane_ : planes_) plane |= plane_;

  return plane;
}

Coords Board::FindPiece(Piece piece) {
  Coords coords;

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      if (GetField(x, y) == piece) coords.push_back({x, y});
    }
  }

  return coords;
}

void Board::SetField(int x, int y, Piece piece) {
  assert(x < width_ && y < height_);

  ClearField(x, y);

  if (piece == kEmptyPiece) return;

  assert(piece.figure < figure_count_ && piece.player < 2);

  planes_[piece.player * figure_count_ + piece.figure].set(x, y);
}

Piece Board::GetField(int x, int y) {
  assert(x < width_ && y < height_);

  for (int i = 0; i < static_cast<int>(planes_.size()); ++i) {
    if (!planes_[i].get(x, y)) continue;
    return Piece{i % figure_count_, i / figure_count_};
  }

  return kEmptyPiece;
}

void Board::ClearField(int x, int y) {
  assert(x < width_ && y < height_);

  for (int i = 0; i < static_cast<int>(planes_.size()); ++i)
    planes_[i].clear(x, y);
}

void Board::MoveField(int x, int y, int x_, int y_) {
  Piece piece = GetField(x, y);
  ClearField(x, y);
  SetField(x_, y_, piece);
}

void Board::Rotate() {
  for (BoardPlane& plane : planes_) plane.Rotate();
}

std::vector<char> Board::ToBytes() {
  struct BoardByteRepr board_struct;

  board_struct.width = int(width_);
  board_struct.height = int(height_);
  board_struct.figure_count = int(figure_count_);

  std::vector<char> output(sizeof board_struct);

  std::memcpy(&output[0], static_cast<void*>(&board_struct),
              sizeof board_struct);

  for (auto plane : planes_) {
    auto plane_bytes = plane.ToBytes();
    output.insert(output.end(), plane_bytes.begin(), plane_bytes.end());
  }

  return output;
}

std::tuple<Board, int> Board::FromBytes(std::vector<char> bytes) {
  int bytes_read = 0;

  // Gather board settings
  struct BoardByteRepr* board_struct =
      static_cast<struct BoardByteRepr*>(static_cast<void*>(&bytes[0]));
  bytes_read += sizeof *board_struct;

  assert(board_struct->width > 0 && board_struct->width < 256);
  assert(board_struct->height > 0 && board_struct->height < 256);
  assert(board_struct->figure_count > 0 && board_struct->figure_count < 256);

  Board board(board_struct->width, board_struct->height,
              board_struct->figure_count);

  // Collect board planes
  std::vector<BoardPlane> planes;
  BoardPlane plane;

  for (int i = 0; i < int(board.GetFigureCount()) * 2; ++i) {
    std::vector<char> plane_bytes(bytes.begin() + bytes_read, bytes.end());
    auto result = BoardPlane::FromBytes(plane_bytes);

    planes.push_back(std::get<0>(result));
    bytes_read += std::get<1>(result);
  }
  board.planes_ = planes;

  return std::make_tuple(board, bytes_read);
}

torch::Tensor Board::AsTensor() const {
  torch::Tensor first_plane_as_tensor = planes_[0].AsTensor();
  torch::IntArrayRef shape = first_plane_as_tensor.sizes();
  torch::Tensor res =
      torch::reshape(first_plane_as_tensor, {1, shape[0], shape[1]});

  int num_planes = planes_.size();
  for (int i = 1; i < num_planes; ++i) {
    res = torch::cat(
        {res, torch::reshape(planes_[i].AsTensor(), {1, shape[0], shape[1]})});
  }
  return res;
}

}  // namespace aithena
