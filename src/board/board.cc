#include "board/board.h"

namespace aithena {

Board::Board(std::size_t width, std::size_t height, unsigned char figure_count)
  : planes_(figure_count * 2, BoardPlane(width, height)),
    width_(width),
    height_(height),
    figure_count_(figure_count) {}

}
