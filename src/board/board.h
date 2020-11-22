#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <vector>

#include "board/board_plane.h"

namespace aithena {

// The class manages two board planes for each figure, one for each player,
// that represent the state of a game's board.
// Player and Figure identifiers must be continuous series from zero up to
// 2 and figure_count respectively.
class Board {
 public:
  // Creates a board plane of size width x height for each type of figure and
  // each player (2).
  Board(std::size_t width, std::size_t height, unsigned char figure_count);

  BoardPlane GetPlane(unsigned char figure, unsigned char player);
 private:
  std::size_t width_, height_;
  unsigned char figure_count_;
  std::vector<BoardPlane> planes_;
};

}

#endif // AITHENTA_BOARD_BOARD_H
