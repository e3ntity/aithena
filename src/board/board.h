#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <vector>

#include "board/board_plane.h"

namespace aithena {

class Board {
 public:
  // Creates a board plane of size width x height for each type of figure and
  // player
  Board(std::size_t width, std::size_t height, unsigned char figure_count);
  Board(Board&);

  BoardPlane GetPlane(unsigned char figure);
 private:
  std::size_t width_, height_;
  unsigned char figure_count_;
  std::vector<BoardPlane> planes_;
};

}

#endif // AITHENTA_BOARD_BOARD_H
