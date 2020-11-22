#include "board/board.h"

namespace aithena {

Board::Board(std::size_t width, std::size_t height, unsigned char figure_count)
  : planes_(figure_count * 2, BoardPlane(width, height)),
    width_(width),
    height_(height),
    figure_count_(figure_count) {}

Board& Board::operator=(const Board& other) {
  if (&other == this) return *this;

  planes_ = other.planes_;
  width_ = other.width_;
  height_ = other.height_;
  figure_count_ = other.figure_count_;

  return *this;
};

}
