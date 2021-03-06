/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_CHESS_STATE_H_
#define SRC_CHESS_STATE_H_

#include "game/state.h"
#include "chess/game.h"

namespace aithena {
namespace chess {

class State : public ::aithena::State {
 public:
  // Initializes to the chess starting state.
  State(std::size_t width, std::size_t height, unsigned figure_count);
  // Initializes to a give state.
  State(const State&);

  State& operator=(const State&);

  // Checks whether both states have the same board, it is the same player's
  // turn and the same castling moves are still allowed.
  bool operator==(const State&);
  bool operator!=(const State&);

  Player GetPlayer();
  Player GetOpponent();
  void SetPlayer(Player);

  bool GetCastleQueen(Player);
  bool GetCastleKing(Player);
  void SetCastleQueen(Player);
  void SetCastleKing(Player);

  unsigned int GetMoveCount();
  void IncMoveCount();

  unsigned int GetNoProgressCount();
  void IncNoProgressCount();
  void ResetNoProgressCount();

  unsigned GetDPushPawnX();
  unsigned GetDPushPawnY();
  void SetDPushPawnX(unsigned);
  void SetDPushPawnY(unsigned);

 private:
  // Indicates the player that has to make the next move.
  Player player_;
  // Indicates for each player whether queen-side castling is allowed.
  std::array<bool, 2> castle_queen_;
  // Indicates for each player whether king-side castling is allowed.
  std::array<bool, 2> castle_king_;
  // The count of moves made up until this state.
  unsigned int move_count_;
  // Counts the moves without progress (neither a pawn moved, nor a piece
  // captured) up util this state.
  unsigned int no_progress_count_;
  signed double_push_pawn_x;
  signed double_push_pawn_y;
};

}  // namespace chess
}  // namespace aithena

#endif  // SRC_CHESS_STATE_H_
