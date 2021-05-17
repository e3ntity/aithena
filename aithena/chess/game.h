/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_CHESS_GAME_H_
#define AITHENA_CHESS_GAME_H_

#include "game/game.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>

#include "benchmark/benchmark.h"
#include "chess/moves.h"
#include "chess/piece.h"
#include "chess/state.h"

namespace aithena {
namespace chess {

class Game : public ::aithena::Game<State> {
 public:
  using GamePtr = std::shared_ptr<Game>;

  Game();
  explicit Game(Options);
  Game(const Game&);

  Game& operator=(const Game&);

  State::StatePtr GetInitialState() override;
  std::vector<State::StatePtr> GetLegalActions(State::StatePtr) override;

  bool IsTerminalState(State::StatePtr) override;
  int GetStateResult(State::StatePtr) override;

  // Returns whether the king of the player, whose turn it is, is in check.
  bool KingInCheck(State state);

  // Takes in a previous state and a list of next-states from that state and
  // updates player turn, no progress counter, and turn counter.
  std::vector<State> PreparePseudoMoves(State state, std::vector<State> moves);

  // Generates all pseudo-moves for all pieces for a given state.
  std::vector<State> GenPseudoMoves(State);
  // Generates all pseudo-moves for any piece at field (x, y) for a given state.
  std::vector<State> GenPseudoMoves(State, int x, int y);

  // Generates all legal moves for a given state.
  std::vector<State::StatePtr> GenMoves(State::StatePtr);

  // Generates the next moves for a single pawn at field (x, y) for a given
  // state.
  // * Assumes that there is a pawn of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenPawnMoves(State, int x, int y);
  std::vector<State> GenPawnPushes(State, int x, int y);
  std::vector<State> GenPawnCaptures(State, int x, int y);

  // Same as GenPawnCaptures but does not check whether there is a piece to
  // capture. Does not set move_info_ field in states!
  std::vector<State> GenRawPawnCaptures(State, int x, int y);

  // Generates the next moves for a single rook at field (x, y) for a given
  // state.
  // * Assumes that there is a pawn of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenRookMoves(State state, int x, int y);

  // Generates the next moves for a single bishop at field (x, y) for a given
  // state.
  // * Assumes that there is a rook of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenBishopMoves(State state, int x, int y);

  // Generates the next moves for a single knight at field (x, y) for a given
  // state.
  // * Assumes that there is a knight of the player who's turn it is at
  // field (x, y).
  // * Does not switch the player turn.
  std::vector<State> GenKnightMoves(State state, int x, int y);

  // Generates the next moves for a single queen at field (x, y) for a given
  // state.
  std::vector<State> GenQueenMoves(State state, int x, int y);

  // Generates the next moves for a single king at field (x, y) for a given
  // state. This does not include castling moves (use GenCastlingMoves).
  std::vector<State> GenKingMoves(State state, int x, int y);

  // Generates castling moves for a piece at (x, y), assumed to be a king.
  std::vector<State> GenCastlingMoves(State state, int x, int y);

  // Returns a board plane highlighting all the squares that contain pieces
  // attacking the piece at field (x, y).
  // Does not take en-passant moves into account!
  BoardPlane GetAttackers(State state, int x, int y);

  // Returns a board plane highlighting all the squares with pieces that are
  // pinned to the piece at field (x, y).

  // Returns a vector of coord tuples indicating pins, with the first coordinate
  // entry highlighting the pinning piece and the second entry highlighting the
  // pinned piece.
  std::vector<std::tuple<Coord, Coord>> GetPins(State state, int x, int y);

  // Returns whether the move represented by a change from board b1 to board b2
  // is a capture move.
  bool IsCapture(State& state);

  // Returns whether the move represented by a change from state s1 to state s2
  // is an en-passant move.
  bool IsEnPassant(const State& state);

  // An array of all figures.
  static const std::array<Figure, 6> figures;

  // An array of all slider figures.
  static const std::array<Figure, 3> slider_figures;

  // An array of all players.
  static const std::array<Player, 2> players;

  static const int figure_count;
  static const int player_count;

  BenchmarkSet benchmark_gen_moves_;
  BenchmarkSet benchmark_;

 private:
  // Returns the x-coordinates of the rooks available for castling.
  std::tuple<Coord, Coord> GetCastlingRooks(State state);
  // Helper function for filtering out en-passant discovered check moves
  bool IsEnPassantDiscoveredCheck(State& s1, State& s2);
  // Flips the player to player of all pieces but the one specified by place.
  State FlipMostPieces(State state, Player player, Coord place);

  // Stores the magic bit boards computed by InitializeMagic.
  // magic_bit_planes_[2*i + 0] thereby stores the push bit planes for figure i
  // whereas magic_bit_planes[2*i + 1] stores the capture bit planes
  std::array<std::unique_ptr<BoardPlane[]>,
             static_cast<int>(Figure::kCount) * 2>
      magic_bit_planes_;
  // For faster access to GetOption("max_no_progress")
  int max_no_progress_;
  // For faster access to GetOption("max_move_count")
  // Set to 0 to disable.
  int max_move_count_;
};

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_GAME_H_
