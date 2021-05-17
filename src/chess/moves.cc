/*
Copyright 2020 All rights reserved.
*/

#include "chess/moves.h"

#include <memory>

namespace aithena {
namespace chess {

Direction operator+(Direction d1, Direction d2) {
  return {d1.x + d2.x, d1.y + d2.y};
}

std::vector<State> GenDirectionalMoves(State state, int x, int y,
                                       std::vector<Direction> directions,
                                       int range) {
  std::vector<State> moves;  // return value

  Board& board = state.GetBoard();
  int width = board.GetWidth();
  int height = board.GetHeight();

  for (auto direction : directions) {
    for (int distance = 1; distance <= range; ++distance) {
      int new_x = x + distance * direction.x;
      int new_y = y + distance * direction.y;

      if (new_x >= width || new_x < 0 || new_y >= height || new_y < 0)
        // Out of bounds
        break;

      Piece piece = board.GetField(new_x, new_y);

      if (piece.player == state.GetPlayer())
        // Blocked by own piece
        break;

      State new_state{state};
      new_state.SetDPushPawn({-1, -1});
      new_state.GetBoard().MoveField(x, y, new_x, new_y);

      new_state.move_info_ = std::make_shared<MoveInfo>(
          Coord({x, y}), Coord({new_x, new_y}), 0, 0, 0);

      if (!(piece == kEmptyPiece)) new_state.move_info_->SetCapture(true);

      moves.push_back(new_state);

      // Do not move further in direction on capture
      if (piece.player == state.GetOpponent()) break;
    }
  }

  return moves;
}  // namespace chess

}  // namespace chess
}  // namespace aithena
