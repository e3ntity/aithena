#ifndef AITHENTA_BOARD_BOARD_H
#define AITHENTA_BOARD_BOARD_H

#include <climits>
#include <vector>

#include "board/board_plane.h"

namespace aithena {

// Represents a piece (figure, player) on the chess board.
struct Piece {
  unsigned figure;
  unsigned player;
};

const Piece kEmptyPiece = {UINT_MAX, UINT_MAX};

bool operator==(const Piece&, const Piece&);

// The class manages two board planes for each figure, one for each player,
// that represent the state of a game's board.
// Player and Figure identifiers must be continuous series from zero up to
// 2 and figure_count respectively.
class Board {
public:
  // Creates a board plane of size width x height for each type of figure and
  // each player (2).
  Board(std::size_t width, std::size_t height, unsigned figure_count);
  Board(const Board&);

  Board& operator=(const Board&);

  std::size_t GetWidth();
  std::size_t GetHeight();

  BoardPlane GetPlane(unsigned figure, unsigned player);
  void SetPiece(unsigned x, unsigned y, Piece);
  // Returns a tuple (figure, player) or (-1, -1) if the field is empty.
  Piece GetPiece(unsigned x, unsigned y);
private:
  std::size_t width_, height_;
  unsigned figure_count_;
  std::vector<BoardPlane> planes_;
};

}

#endif // AITHENTA_BOARD_BOARD_H
