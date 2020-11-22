#include "board/board.h"

namespace aithena {

Board::Board(std::size_t width, std::size_t height, unsigned figure_count)
  : planes_(figure_count * 2, BoardPlane(width, height)),
    width_(width),
    height_(height),
    figure_count_(figure_count) {}

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

Piece Board::GetPiece(unsigned x, unsigned y) {
  for (unsigned i = 0; i < planes_.size(); ++i) {
    if (!planes_[i].get(x, y)) continue;
    return Piece{i % figure_count_, unsigned(i / figure_count_)};
  }

  return kEmptyPiece;
}

bool operator==(const Piece& a, const Piece& b) {
  return a.figure == b.figure && a.player == b.player;
}

}
