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

// Piece value returned for a square without a piece.
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
  unsigned GetFigureCount();

  // Returns the BoardPlane for a given piece.
  BoardPlane& GetPlane(Piece);
  // Returns the BoardPlane for a given figure (OR'd over players)
  BoardPlane GetFigurePlane(unsigned);
  // Returns the BoardPlane for a given player (OR'd over figures)
  BoardPlane GetPlayerPlane(unsigned);
  BoardPlane GetCompletePlane();

  // Sets the piece of field (x, y). If piece is kEmptyPiece, the field is
  // simply cleared.
  void SetField(unsigned x, unsigned y, Piece);
  // Returns the piece that is on field (x, y). If the field is empty,
  // kEmptyPiece is returned.
  Piece GetField(unsigned x, unsigned y);
  // Removes any figure from the field (x, y). If the field is already empty,
  // nothing happens.
  void ClearField(unsigned x, unsigned y);
  // Moves The piece on field (x, y) to field (x_, y_) thereby possibly
  // overriding a piece on the destination position.
  void MoveField(unsigned x, unsigned y, unsigned x_, unsigned y_);

  friend Board BoardDifference(Board&, Board&);

 private:
  // The width and height of the board and therefore of all board planes
  // managed by the board.
  std::size_t width_, height_;
  // The number of figures managed by the board.
  unsigned figure_count_;
  // The board planes managed by the board, two for each piece (one for each
  // player).
  std::vector<BoardPlane> planes_;
};

Board BoardDifference(Board&, Board&);

}

#endif // AITHENTA_BOARD_BOARD_H
