#include "game/state.h"

#include <string>
#include <sstream>
#include <iostream>

namespace aithena {

State::State(std::size_t width, std::size_t height, unsigned char figure_count)
  : board_{width, height, figure_count} {}

State::State(const State& other) : board_(other.board_) {}

State& State::operator=(const State& other) {
  if (this == &other) return *this;

  board_ = other.board_;

  return *this;
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
};

Board& State::GetBoard() {return board_;}

std::string State::PrintBoard() {
  std::ostringstream repr;
  Piece piece;

  for (int y = board_.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << " " << std::string(board_.GetWidth() * 4 + 1, '-')
         << std::endl << y << "|";
    for (unsigned x = 0; x < board_.GetWidth(); ++x) {
      piece = board_.GetField(x, y);

      if (piece == kEmptyPiece) repr << "   |";
      else repr << " " << piece.figure << " |";
    }
  }
  repr << std::endl << " " << std::string(board_.GetWidth() * 4 + 1, '-')
       << std::endl << "   A   B   C   D   E   F   G   H" << std::endl;

  return repr.str();
}

}
