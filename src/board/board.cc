#include "board/board.h"

namespace aithena {

bool operator==(const Piece& a, const Piece& b) {
  return a.figure == b.figure && a.player == b.player;
}

Board::Board(std::size_t width, std::size_t height, unsigned figure_count)
  : width_(width),
    height_(height),
    figure_count_(figure_count),
    planes_(figure_count * 2, BoardPlane(width, height)) {}
Board::Board(const Board& other) {*this = other;}

Board& Board::operator=(const Board& other) {
  if (&other == this) return *this;

  planes_ = other.planes_;
  width_ = other.width_;
  height_ = other.height_;
  figure_count_ = other.figure_count_;

  return *this;
};

std::size_t Board::GetWidth() {return width_;}
std::size_t Board::GetHeight() {return height_;}

BoardPlane& Board::GetPlane(Piece piece) {
  assert(piece.figure < figure_count_ && piece.player < 2);
  return planes_[piece.player * figure_count_ + piece.figure];
}

void Board::SetField(unsigned x, unsigned y, Piece piece) {
  assert(x < width_ && y < height_);

  ClearField(x, y);

  if (piece == kEmptyPiece)
    return;

  assert(piece.figure < figure_count_ && piece.player < 2);

  planes_[piece.player * figure_count_ + piece.figure].set(x, y);
}

Piece Board::GetField(unsigned x, unsigned y) {
  assert(x < width_ && y < height_);

  for (unsigned i = 0; i < planes_.size(); ++i) {
    if (!planes_[i].get(x, y)) continue;
    return Piece{i % figure_count_, unsigned(i / figure_count_)};
  }

  return kEmptyPiece;
}

void Board::ClearField(unsigned x, unsigned y) {
  assert(x < width_ && y < height_);

  for (unsigned i = 0; i < planes_.size(); ++i)
    planes_[i].clear(x, y);
}

void Board::MoveField(unsigned x, unsigned y, unsigned x_, unsigned y_) {
  Piece piece = GetField(x, y);
  ClearField(x, y);
  SetField(x_, y_, piece);
}

}
