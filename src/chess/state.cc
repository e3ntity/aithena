/*
Copyright 2020 All rights reserved.
*/

#include "chess/state.h"

#include "chess/game.h"

namespace aithena {
namespace chess {

State::State(
  std::size_t width,
  std::size_t height,
  unsigned figure_count = static_cast<unsigned>(Figure::kCount))
  : ::aithena::State(width, height, figure_count),
    player_{Player::kWhite},
    castle_queen_{true, true},
    castle_king_{true, true},
    move_count_{0},
    no_progress_count_{0},
    double_push_pawn_x{ -1},
    double_push_pawn_y{ -1} {}

State::State(const State& other)
  : ::aithena::State(other),
    player_{other.player_},
    castle_queen_{other.castle_queen_},
    castle_king_{other.castle_king_},
    move_count_{other.move_count_},
    no_progress_count_{other.no_progress_count_},
    double_push_pawn_x{other.double_push_pawn_x},
    double_push_pawn_y{other.double_push_pawn_y} {}

State& State::operator=(const State& other) {
  if (this == &other) return *this;

  ::aithena::State::operator=(other);

  player_ = other.player_;
  castle_queen_ = other.castle_queen_;
  castle_king_ = other.castle_king_;
  move_count_ = other.move_count_;
  no_progress_count_ = other.no_progress_count_;
  double_push_pawn_x = other.double_push_pawn_x;
  double_push_pawn_y = other.double_push_pawn_y;

  return *this;
}

bool State::operator==(const State& other) {
  return ::aithena::State::operator==(other)
         && player_ == other.player_
         && castle_queen_ == other.castle_queen_
         && castle_king_ == other.castle_king_
         && double_push_pawn_x == other.double_push_pawn_x
         && double_push_pawn_y == other.double_push_pawn_y;
}

bool State::operator!=(const State& other) {
  return !operator==(other);
}

Player State::GetPlayer() { return player_; }
void State::SetPlayer(Player p) { player_ = p; }

bool State::GetCastleQueen(Player p) {
  return castle_queen_[static_cast<unsigned>(p)];
}
bool State::GetCastleKing(Player p) {
  return castle_king_[static_cast<unsigned>(p)];
}
void State::SetCastleQueen(Player p) {
  castle_queen_[static_cast<unsigned>(p)] = false;
}
void State::SetCastleKing(Player p) {
  castle_king_[static_cast<unsigned>(p)] = false;
}

unsigned State::GetMoveCount() { return move_count_; }
void State::IncMoveCount() { ++move_count_; }

unsigned State::GetNoProgressCount() { return no_progress_count_; }
void State::IncNoProgressCount() { ++no_progress_count_; }
void State::ResetNoProgressCount() { no_progress_count_ = 0; }


unsigned State::GetDPushPawnX() { return double_push_pawn_x; }

unsigned State::GetDPushPawnY() { return double_push_pawn_y; }

void State::SetDPushPawnX(unsigned x) { double_push_pawn_x = x; }

void State::SetDPushPawnY(unsigned y) { double_push_pawn_y = y; }

}  // namespace chess
}  // namespace aithena
