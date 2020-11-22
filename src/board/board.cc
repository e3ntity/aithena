#include "board/board.h"

namespace aithena {

Board::Board(std::size_t width, std::size_t height, unsigned char figure_count)
  : planes_(figure_count, BoardPlane(width, height)),
    width_(width),
    height_(height),
    figure_count_(figure_count) {}

Board::Board(Board& other)
  : planes_(other.figure_count_, BoardPlane(other.width_, other.height_)),
    width_(other.width_),
    height_(other.height_),
    figure_count_(other.figure_count_) {};

}
