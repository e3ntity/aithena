/**
 * Copyright (C) 2020 All Right Reserved
 */

#include "chess/util.h"

#include <string>

#include "board/board.h"
#include "chess/game.h"

namespace aithena {
namespace chess {

std::string PrintMarkedBoard(State::StatePtr state, BoardPlane marker, std::string marker_color) {
  Board board = state->GetBoard();
  std::ostringstream repr;
  Piece piece;
  std::vector<std::string> letters = {"A", "B", "C", "D", "E", "F", "G", "H"};

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << " " << y + 1 << "|";
    for (int x = 0; x < board.GetWidth(); ++x) {
      piece = board.GetField(x, y);
      std::string piece_symbol = (piece == kEmptyPiece ? "." : std::string(1, GetPieceSymbol(piece)));

      bool marked = marker.get(x, y);

      if (marked) repr << marker_color;

      repr << piece_symbol << " \033[0m";
    }
  }

  repr << std::endl << "   " << std::string(static_cast<int>(board.GetWidth()) * 2 - 1, '-');
  repr << std::endl << "  ";
  for (int i = 0; i < static_cast<int>(board.GetWidth()); ++i) repr << " " << letters.at(i);

  repr << std::endl;

  repr << "turn: " << (state->GetPlayer() == Player::kWhite ? "White" : "Black") << std::endl;
  repr << "move: " << state->GetMoveCount() << ", half moves: " << state->GetNoProgressCount() << std::endl;

  return repr.str();
}

std::string PrintBoard(State::StatePtr state) {
  Board& board = state->GetBoard();
  BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

int perft(std::shared_ptr<Game> game, chess::State::StatePtr state, int depth) {
  if (depth == 0) return 1;

  int counter = 0;
  auto moves = game->GetLegalActions(state);

  for (auto move : moves) counter += perft(game, move, depth - 1);

  if (depth > 1) counter += moves.size();

  return counter;
}

std::vector<std::tuple<State::StatePtr, int>> divide(std::shared_ptr<Game> game, State::StatePtr state, int depth) {
  std::vector<std::tuple<State::StatePtr, int>> output;
  auto moves = game->GenMoves(state);

  for (auto move : moves) output.push_back(std::make_tuple(move, perft(game, move, depth - 1)));

  return output;
}

}  // namespace chess
}  // namespace aithena
