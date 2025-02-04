/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_BOARD_BOARD_H_
#define AITHENA_BOARD_BOARD_H_

#include <torch/torch.h>
#include <climits>
#include <vector>

#include "board/board_plane.h"

namespace aithena {

// Represents a piece (figure, player) on the chess board.
struct Piece {
  int figure;
  int player;
};

// Piece value returned for a square without a piece.
constexpr Piece kEmptyPiece = {INT_MAX, INT_MAX};

bool operator==(const Piece&, const Piece&);

// The class manages two board planes for each figure, one for each player,
// that represent the state of a game's board.
// Player and Figure identifiers must be continuous series from zero up to
// 2 and figure_count respectively.
class Board {
 public:
  // Creates a board plane of size width x height for each type of figure and
  // each player (2).
  Board(int width, int height, int figure_count);
  Board(const Board&);

  Board& operator=(const Board&);

  // Checks that both boards have the same dimension, number of figures and
  // whether the plane for each figure is the same.
  bool operator==(const Board&) const;
  bool operator!=(const Board&) const;

  int GetWidth();
  int GetHeight();
  int GetFigureCount();

  // Returns the BoardPlane for a given piece.
  BoardPlane& GetPlane(Piece);
  BoardPlane GetFigurePlane(int figure);
  BoardPlane GetPlayerPlane(int player);
  BoardPlane GetCompletePlane();

  // Returns the coordinates of all pieces of some kind in format (x, y).
  Coords FindPiece(Piece);

  // Sets the piece of field (x, y). If piece is kEmptyPiece, the field is
  // simply cleared.
  void SetField(int x, int y, Piece);
  // Returns the piece that is on field (x, y). If the field is empty,
  // kEmptyPiece is returned.
  Piece GetField(int x, int y);
  // Removes any figure from the field (x, y). If the field is already empty,
  // nothing happens.
  void ClearField(int x, int y);
  // Moves The piece on field (x, y) to field (x_, y_) thereby possibly
  // overriding a piece on the destination position.
  void MoveField(int x, int y, int x_, int y_);
  // Rotates the board by 180 degrees.
  void Rotate();

  // Returns a tensor representatio of the board.
  torch::Tensor AsTensor() const;

  // Converts the board to bytes.
  std::vector<char> ToBytes();
  // Reads a byte representation of the board into a board object. Additionally
  // returns the number of bytes read.
  static std::tuple<Board, int> FromBytes(std::vector<char>);

  friend BoardPlane GetNewFields(const Board&, const Board&);

 private:
  // The width and height of the board and therefore of all board planes
  // managed by the board.
  int width_, height_;
  // The number of figures managed by the board.
  int figure_count_;
  // The board planes managed by the board, two for each piece (one for each
  // player).
  std::vector<BoardPlane> planes_;

  struct BoardByteRepr {
    int width, height, figure_count;
  };
};

// Returns a board plane highlighting all fields that contain a different
// figure in the after board than they did in the before board.
// If a figure was removed and is now kEmptyPiece, no indication is given.
BoardPlane GetNewFields(const Board& before, const Board& after);

}  // namespace aithena

#endif  // AITHENA_BOARD_BOARD_H_
