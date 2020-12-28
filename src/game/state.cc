/*
Copyright 2020 All rights reserved.
*/

#include "game/state.h"

#include <iostream>
#include <string>
#include <sstream>

namespace aithena {

State::State(std::size_t width, std::size_t height, unsigned char figure_count)
  : board_{width, height, figure_count} {}

State::State(const State& other) : board_(other.board_) {}

State& State::operator=(const State& other) {
  board_ = other.board_;

  return *this;
}

bool State::operator==(const State& other) {
  return board_ == other.board_;
}

bool State::operator!=(const State& other) {
  return !operator==(other);
}

// Prints out the state in the following format:
// figure-player:x-y/...
std::string State::ToString() {
  std::ostringstream repr;
  Piece piece;

  for (unsigned x = 0; x < board_.GetWidth(); ++x) {
    for (unsigned y = 0; y < board_.GetHeight(); ++y) {
      piece = board_.GetField(x, y);
      if (piece == kEmptyPiece) continue;
      repr << piece.figure << "-" << piece.player << ":"
           << x << "-" << y << "/";
    }
  }

  return repr.str();
}

Board& State::GetBoard() {return board_;}

}  // namespace aithena
