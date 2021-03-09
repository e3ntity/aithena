/*
Copyright 2020 All rights reserved.
*/

#include "chess/state.h"

#include <torch/torch.h>
#include <cstring>

#include "chess/game.h"

namespace aithena {
namespace chess {

State::State(std::size_t width, std::size_t height,
             unsigned figure_count = static_cast<unsigned>(Figure::kCount))
    : ::aithena::State(width, height, figure_count),
      player_{Player::kWhite},
      castle_queen_{false, false},
      castle_king_{false, false},
      move_count_{0},
      no_progress_count_{0},
      double_push_pawn_x{-1},
      double_push_pawn_y{-1} {}

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
  return ::aithena::State::operator==(other) && player_ == other.player_ &&
         castle_queen_ == other.castle_queen_ &&
         castle_king_ == other.castle_king_ &&
         double_push_pawn_x == other.double_push_pawn_x &&
         double_push_pawn_y == other.double_push_pawn_y;
}

bool State::operator!=(const State& other) { return !operator==(other); }

Player State::GetPlayer() { return player_; }
Player State::GetOpponent() {
  return player_ == Player::kWhite ? Player::kBlack : Player::kWhite;
}
void State::SetPlayer(Player p) { player_ = p; }

bool State::GetCastleQueen(Player p) {
  return castle_queen_[static_cast<unsigned>(p)];
}
bool State::GetCastleKing(Player p) {
  return castle_king_[static_cast<unsigned>(p)];
}
void State::SetCastleQueen(Player p) {
  castle_queen_[static_cast<unsigned>(p)] = true;
}
void State::SetCastleKing(Player p) {
  castle_king_[static_cast<unsigned>(p)] = true;
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

torch::Tensor State::PlanesAsTensor() {
  unsigned width = GetBoard().GetWidth();
  unsigned height = GetBoard().GetHeight();

  torch::Tensor board_tensor = GetBoard().AsTensor();
  torch::IntArrayRef shape = board_tensor.sizes();

  torch::Tensor repetition0 = torch::zeros({1, width, height});
  torch::Tensor repetition1 = torch::zeros({1, width, height});

  return torch::cat({board_tensor, repetition1, repetition0})
      .reshape({1, shape[0] + 2, shape[1], shape[2]});
}

torch::Tensor State::DetailsAsTensor() {
  unsigned width = GetBoard().GetWidth();
  unsigned height = GetBoard().GetHeight();

  torch::Tensor color;
  torch::Tensor move_count;
  torch::Tensor p1castling;
  torch::Tensor p2castling;
  torch::Tensor no_progress_count;

  if (GetPlayer() == Player::kBlack)
    color = torch::zeros({1, width, height});
  else
    color = torch::ones({1, width, height});

  move_count = torch::reshape(BoardPlane(count_lut[move_count_])
                                  .AsTensor()
                                  .slice(0, 0, width)
                                  .slice(1, 0, height),
                              {1, width, height});

  if (castle_queen_[0])
    p1castling = torch::ones({1, width, height});
  else
    p1castling = torch::zeros({1, width, height});

  if (castle_king_[0])
    p1castling = torch::cat({p1castling, torch::ones({1, width, height})});
  else
    p1castling = torch::cat({p1castling, torch::zeros({1, width, height})});

  if (castle_queen_[1])
    p2castling = torch::ones({1, width, height});
  else
    p2castling = torch::zeros({1, width, height});

  if (castle_king_[1])
    p2castling = torch::cat({p2castling, torch::ones({1, width, height})});
  else
    p2castling = torch::cat({p2castling, torch::zeros({1, width, height})});

  no_progress_count = torch::reshape(BoardPlane(count_lut[no_progress_count_])
                                         .AsTensor()
                                         .slice(0, 0, width)
                                         .slice(1, 0, height),
                                     {1, width, height});

  return torch::cat(
             {color, move_count, p1castling, p2castling, no_progress_count})
      .reshape({1, 7, width, height});
}

std::vector<char> State::ToBytes() {
  struct StateByteRepr state_struct;

  state_struct.player = static_cast<int>(player_);
  state_struct.move_count = static_cast<int>(move_count_);
  state_struct.no_progress_count = static_cast<int>(no_progress_count_);
  state_struct.double_push_pawn[0] = static_cast<int>(double_push_pawn_x);
  state_struct.double_push_pawn[1] = static_cast<int>(double_push_pawn_y);
  state_struct.castle[0] = castle_queen_[0];
  state_struct.castle[1] = castle_queen_[1];
  state_struct.castle[2] = castle_king_[0];
  state_struct.castle[3] = castle_king_[1];

  std::vector<char> output(sizeof state_struct);

  std::memcpy(&output[0], static_cast<void*>(&state_struct),
              sizeof state_struct);

  std::vector<char> board_output = board_.ToBytes();

  output.insert(output.end(), board_output.begin(), board_output.end());

  return output;
}

std::tuple<::aithena::chess::State, int> State::FromBytes(
    std::vector<char> bytes) {
  int bytes_read = 0;

  // Gather state settings
  struct StateByteRepr* state_struct =
      static_cast<struct StateByteRepr*>(static_cast<void*>(&bytes[0]));
  bytes_read += sizeof *state_struct;

  // Gather board
  std::vector<char> board_bytes(bytes.begin() + bytes_read, bytes.end());
  std::tuple<Board, int> board_return = Board::FromBytes(board_bytes);

  Board board = std::get<0>(board_return);
  bytes_read += std::get<1>(board_return);

  // Create new state
  State state(board.GetWidth(), board.GetHeight(), board.GetFigureCount());
  state.board_ = board;

  state.player_ = static_cast<Player>(state_struct->player);
  state.castle_queen_[0] = state_struct->castle[0];
  state.castle_queen_[1] = state_struct->castle[1];
  state.castle_king_[0] = state_struct->castle[2];
  state.castle_king_[1] = state_struct->castle[3];
  state.move_count_ = static_cast<unsigned>(state_struct->move_count);
  state.no_progress_count_ =
      static_cast<unsigned>(state_struct->no_progress_count);
  state.double_push_pawn_x = state_struct->double_push_pawn[0];
  state.double_push_pawn_y = state_struct->double_push_pawn[1];

  return std::make_tuple(state, bytes_read);
}

}  // namespace chess
}  // namespace aithena
