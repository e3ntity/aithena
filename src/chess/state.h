/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_CHESS_STATE_H_
#define SRC_CHESS_STATE_H_

#include <torch/torch.h>

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

  torch::Tensor PlanesAsTensor();
  torch::Tensor DetailsAsTensor();

  std::vector<char> ToBytes();
  static std::tuple<::aithena::chess::State, int> FromBytes(std::vector<char>);

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

  struct StateByteRepr {
    int player, move_count, no_progress_count, double_push_pawn[2];
    bool castle[4];
  };
};

constexpr uint64_t count_lut[] = {0x0,
                                  0x1,
                                  0x3,
                                  0x7,
                                  0xf,
                                  0x1f,
                                  0x3f,
                                  0x7f,
                                  0xff,
                                  0x1ff,
                                  0x3ff,
                                  0x7ff,
                                  0xfff,
                                  0x1fff,
                                  0x3fff,
                                  0x7fff,
                                  0xffff,
                                  0x1ffff,
                                  0x3ffff,
                                  0x7ffff,
                                  0xfffff,
                                  0x1fffff,
                                  0x3fffff,
                                  0x7fffff,
                                  0xffffff,
                                  0x1ffffff,
                                  0x3ffffff,
                                  0x7ffffff,
                                  0xfffffff,
                                  0x1fffffff,
                                  0x3fffffff,
                                  0x7fffffff,
                                  0xffffffff,
                                  0x1ffffffff,
                                  0x3ffffffff,
                                  0x7ffffffff,
                                  0xfffffffff,
                                  0x1fffffffff,
                                  0x3fffffffff,
                                  0x7fffffffff,
                                  0xffffffffff,
                                  0x1ffffffffff,
                                  0x3ffffffffff,
                                  0x7ffffffffff,
                                  0xfffffffffff,
                                  0x1fffffffffff,
                                  0x3fffffffffff,
                                  0x7fffffffffff,
                                  0xffffffffffff,
                                  0x1ffffffffffff,
                                  0x3ffffffffffff};

}  // namespace chess
}  // namespace aithena

#endif  // SRC_CHESS_STATE_H_
