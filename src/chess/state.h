/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_CHESS_STATE_H_
#define AITHENA_CHESS_STATE_H_

#include <torch/torch.h>

#include "game/state.h"

#include "chess/game.h"

namespace aithena {
namespace chess {

// Special bit-fields:
// - if promotion bit is set, indicate type of promotion (0: knight, 1: bishop,
//   2: rook, 3: queen)
// - if not promotion and capture bit is set, special == 1 indicates en passant
// - otherwise 0: normal push, 1: double pawn push, 2: k castle, 3: q castle
struct MoveInfo {
  MoveInfo(struct Coord from_, struct Coord to_, unsigned char promotion_,
           unsigned char capture_, unsigned char special_)
      : from{from_},
        to{to_},
        promotion{promotion_},
        capture{capture_},
        special{special_} {}
  Coord from;
  Coord to;
  unsigned char promotion : 1;
  unsigned char capture : 1;
  unsigned char special : 2;
};

class State : public ::aithena::State {
 public:
  using StatePtr = std::shared_ptr<State>;
  // Initializes to the chess starting state.
  State(int width, int height,
        int figure_count = static_cast<int>(Figure::kCount));
  // Initializes to a give state.
  State(const State&);

  State& operator=(const State&);

  // Checks whether both states have the same board, it is the same player's
  // turn and the same castling moves are still allowed.
  // Move information is not taken into account!
  bool operator==(const State&);
  bool operator!=(const State&);

  Player GetPlayer();
  Player GetOpponent();
  void SetPlayer(Player);

  bool GetCastleQueen(Player);
  bool GetCastleKing(Player);
  void SetCastleQueen(Player);
  void SetCastleKing(Player);

  int GetMoveCount();
  void IncMoveCount();
  void SetMoveCount(int);

  int GetNoProgressCount();
  void IncNoProgressCount();
  void ResetNoProgressCount();
  void SetNoProgressCount(int);

  Coord GetDPushPawn();
  int GetDPushPawnX();
  int GetDPushPawnY();
  void SetDPushPawn(Coord);
  void SetDPushPawnX(int);
  void SetDPushPawnY(int);

  torch::Tensor PlanesAsTensor();
  torch::Tensor DetailsAsTensor();

  std::vector<char> ToBytes();

  // Represents the state as a string in FEN notation
  std::string ToFEN();

  // Loads a state from a string, represented in FEN notation.
  // Returns nullptr if the string is invalid.
  static StatePtr FromFEN(std::string);
  static std::tuple<::aithena::chess::State, int> FromBytes(std::vector<char>);

  std::shared_ptr<MoveInfo> move_info_{nullptr};

 private:
  // Indicates the player that has to make the next move.
  Player player_;
  // Indicates for each player whether queen-side castling is allowed.
  std::array<bool, 2> castle_queen_;
  // Indicates for each player whether king-side castling is allowed.
  std::array<bool, 2> castle_king_;
  // The count of moves made up until this state.
  int move_count_;
  // Counts the moves without progress (neither a pawn moved, nor a piece
  // captured) up util this state.
  int no_progress_count_;
  Coord double_push_pawn_;

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

constexpr char fen_figures[] = {'k', 'q', 'r', 'n', 'b', 'p'};
constexpr char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                             'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                             's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_STATE_H_
