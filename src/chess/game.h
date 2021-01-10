/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_CHESS_GAME_H_
#define SRC_CHESS_GAME_H_

#include "game/game.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>

namespace aithena {
namespace chess {

// Defines the figures used in chess. GhostPawn is used to keep track of
// "en-passant".
enum class Figure : unsigned {
  kKing, kQueen, kRook,
  kKnight, kBishop, kPawn,
  kCount
};

enum Player : unsigned {kWhite, kBlack};
static const unsigned bb_per_pos = 12;

}  // namespace chess
}  // namespace aithena

// Needs to be imported here as state needs Figure & Player
#include "chess/state.h"
#include "chess/moves.h"

namespace aithena {
namespace chess {

static const BoardPlane one{1UL};

Piece make_piece(Figure, Player);

class Game : public ::aithena::Game<State> {
 public:
  Game();
  explicit Game(Options);
  Game(const Game&);

  Game& operator=(const Game&);

  // Crops bitboards to the specified size of the board.
  // Bitboards are later used for legal move generation.
  void InitializeMagic();

  State GetInitialState() override;
  std::vector<State> GetLegalActions(State) override;
  void GeneratePositionAttackers(unsigned x, unsigned y,
    State* state, BoardPlane* constrained_to,  unsigned* attacker_count,
    BoardPlane* pinned);

  bool SearchForAttack(unsigned x, unsigned y, State* state,
    BoardPlane* filtered_enemies, unsigned bb_offset, unsigned inv_bb,
    bool lower, BoardPlane* constrained_to, BoardPlane* pinned);

  bool IsTerminalState(State&) override;
  int GetStateResult(State&) override;

  // Generates all enxt moves for all pieces for a given state.
  // The pseudo parameter indicates whether to only generate pseudo moves.
  std::vector<State> GenMoves(State, bool pseudo = false);

  // Generates all next moves for any piece at field (x, y) for a given state.
  // The pseudo parameter indicates whether to only generate pseudo moves.
  std::vector<State> GenMoves(State, unsigned x, unsigned y,
                              bool pseudo = false);

  // Generates the next moves for a single pawn at field (x, y) for a given
  // state.
  // * Assumes that there is a pawn of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenPawnMoves(State, unsigned x, unsigned y);

  // Generates the next moves for a single rook at field (x, y) for a given
  // state.
  // * Assumes that there is a pawn of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenRookMoves(State state, unsigned x, unsigned y);

  // Generates the next moves for a single bishop at field (x, y) for a given
  // state.
  // * Assumes that there is a rook of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenBishopMoves(State state, unsigned x, unsigned y);

  // Generates the next moves for a single knight at field (x, y) for a given
  // state.
  // * Assumes that there is a knight of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenKnightMoves(State state, unsigned x, unsigned y);

  // Generates the next moves for a single queen at field (x, y) for a given
  // state.
  std::vector<State> GenQueenMoves(State state, unsigned x, unsigned y);

  // Generates the next moves for a single king at field (x, y) for a given
  // state.
  std::vector<State> GenKingMoves(State state, unsigned x, unsigned y);

  // An array of all figures.
  static const std::array<Figure, 6> figures;

  // An array of all players.
  static const std::array<Player, 2> players;

 private:
  // Returns whether the player's king is in check.
  bool KingInCheck(State state, Player player);

  // Stores the magic bit boards computed by InitializeMagic.
  std::vector<BoardPlane> magic_bit_planes_;

  std::vector<BoardPlane> king_move_patterns;

  // For faster access to GetOption("max_no_progress")
  unsigned int max_no_progress_;
  // For faster access to GetOption("max_move_count")
  // Set to 0 to disable.
  unsigned int max_move_count_;
};

}  // namespace chess
}  // namespace aithena

#endif  // SRC_CHESS_GAME_H_
