#ifndef AITHENTA_GAME_CHESS_H
#define AITHENTA_GAME_CHESS_H

#include "board/board.h"
#include "game/action.h"
#include "game/game.h"
#include "game/state.h"

namespace aithena {
namespace chess {

// Defines the figures used in chess. GhostPawn is used to keep track of
// "en-passant".
enum class Figure : unsigned {
  /*kKing, kQueen, kRook,
  kBishop, kKnight, */kPawn,
  /*kGhostPawn, */kCount
};

enum class Player : unsigned {kWhite, kBlack};

class State : public ::aithena::State {
 public:
  // Initializes to the chess starting state.
  State(std::size_t width, std::size_t height, unsigned figure_count);
  // Initializes to a give state.
  State(const State&);

  State& operator=(const State&);

  Player GetPlayer();
  void SetPlayer(Player);

  bool GetCastleQueen(Player);
  bool GetCasteKing(Player);
  void SetCastleQueen(Player);
  void SetCastleKing(Player);

  unsigned int GetMoveCount();
  void IncMoveCount();

  unsigned int GetNoProgressCount();
  void ResetNoProgressCount();
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
};

class Action : public ::aithena::Action<State> {
 public:
  using ::aithena::Action<State>::Action;

  std::string ToString() override;
};

class Game : public ::aithena::Game<Action, State> {
 public:
  Game();
  Game(Options);

  // Crops bitboards to the specified size of the board.
  // Bitboards are later used for legal move generation.
  void InitializeMagic();

  State GetInitialState() override;
  ActionList GetLegalActions(State) override;
 private:
  // Stores the magic bit boards computed by InitializeMagic.
  // magic_bit_planes_[2*i + 0] thereby stores the push bit planes for figure i
  // whereas magic_bit_planes[2*i + 1] stores the capture bit planes
  std::array<std::unique_ptr<BoardPlane>,
      static_cast<unsigned>(Figure::kCount) * 2> magic_bit_planes_;
};

Piece make_piece(Figure, Player);

}
}

#endif // AITHENTA_GAME_CHESS_H
