#include "chess/chess.h"

namespace aithena {
namespace chess {

State::State(
  std::size_t width,
  std::size_t height,
  unsigned figure_count = static_cast<unsigned>(Figure::kCount))
  : ::aithena::State(width, height, figure_count),
    player_{Player::kWhite},
    castle_queen_{false, false},
    castle_king_{false, false},
    move_count_{0},
    no_progress_count_{0} {}

State::State(const State& other)
  : ::aithena::State(other),
    player_{other.player_},
    castle_queen_{other.castle_queen_},
    castle_king_{other.castle_king_},
    move_count_{other.move_count_},
    no_progress_count_{other.no_progress_count_} {}

State& State::operator=(const State& other) {
  if (this == &other) return *this;

  ::aithena::State::operator=(other);

  player_ = other.player_;
  castle_queen_ = other.castle_queen_;
  castle_king_ = other.castle_king_;
  move_count_ = other.move_count_;
  no_progress_count_ = other.no_progress_count_;

  return *this;
}

Player State::GetPlayer() {
  return player_;
}

}
}
