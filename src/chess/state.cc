/*
Copyright 2020 All rights reserved.
*/

#include "chess/state.h"

#include <torch/torch.h>
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <cstring>

#include "chess/game.h"

namespace aithena {
namespace chess {

State::State(std::size_t width, std::size_t height, unsigned figure_count)
    : ::aithena::State(width, height, figure_count),
      player_{Player::kWhite},
      castle_queen_{true, true},
      castle_king_{true, true},
      move_count_{0},
      no_progress_count_{0},
      double_push_pawn_{-1, -1} {};

State::State(const State& other)
    : ::aithena::State(other),
      player_{other.player_},
      castle_queen_{other.castle_queen_},
      castle_king_{other.castle_king_},
      move_count_{other.move_count_},
      no_progress_count_{other.no_progress_count_},
      double_push_pawn_{other.double_push_pawn_} {};

State& State::operator=(const State& other) {
  if (this == &other) return *this;

  ::aithena::State::operator=(other);

  player_ = other.player_;
  castle_queen_ = other.castle_queen_;
  castle_king_ = other.castle_king_;
  move_count_ = other.move_count_;
  no_progress_count_ = other.no_progress_count_;
  double_push_pawn_ = other.double_push_pawn_;

  return *this;
}

bool State::operator==(const State& other) {
  return ::aithena::State::operator==(other) && player_ == other.player_ &&
         castle_queen_ == other.castle_queen_ &&
         castle_king_ == other.castle_king_ &&
         double_push_pawn_.x == other.double_push_pawn_.x &&
         double_push_pawn_.y == other.double_push_pawn_.y;
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
  castle_queen_[static_cast<unsigned>(p)] = false;
}
void State::SetCastleKing(Player p) {
  castle_king_[static_cast<unsigned>(p)] = false;
}

unsigned State::GetMoveCount() { return move_count_; }
void State::IncMoveCount() { ++move_count_; }
void State::SetMoveCount(unsigned count) { move_count_ = count; }

unsigned State::GetNoProgressCount() { return no_progress_count_; }
void State::IncNoProgressCount() { ++no_progress_count_; }
void State::ResetNoProgressCount() { no_progress_count_ = 0; }
void State::SetNoProgressCount(unsigned count) { no_progress_count_ = count; };

Coord State::GetDPushPawn() { return double_push_pawn_; }
int State::GetDPushPawnX() { return double_push_pawn_.x; }
int State::GetDPushPawnY() { return double_push_pawn_.y; }
void State::SetDPushPawn(Coord c) { double_push_pawn_ = c; }
void State::SetDPushPawnX(int x) { double_push_pawn_.x = x; }
void State::SetDPushPawnY(int y) { double_push_pawn_.y = y; }

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
  state_struct.double_push_pawn[0] = static_cast<int>(double_push_pawn_.x);
  state_struct.double_push_pawn[1] = static_cast<int>(double_push_pawn_.y);
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
  state.double_push_pawn_.x = state_struct->double_push_pawn[0];
  state.double_push_pawn_.y = state_struct->double_push_pawn[1];

  return std::make_tuple(state, bytes_read);
}

std::string State::ToFEN() {
  std::string output;
  int empty;

  // board configuration

  for (int y = board_.GetHeight() - 1; y >= 0; --y) {
    empty = 0;
    for (int x = 0; x < board_.GetWidth(); ++x) {
      Piece piece = board_.GetField(x, y);

      if (piece == kEmptyPiece) {
        ++empty;
        continue;
      }

      if (empty > 0) {
        output += std::to_string(empty);
        empty = 0;
      }

      char fen_figure = fen_figures[static_cast<int>(piece.figure)];

      if (piece.player == Player::kWhite) fen_figure = std::toupper(fen_figure);

      output += fen_figure;
    }

    if (empty > 0) output += std::to_string(empty);
    if (y > 0) output += "/";
  }

  // player

  if (player_ == Player::kWhite)
    output += " w ";
  else
    output += " b ";

  // castling

  bool castling = false;
  if (castle_king_[static_cast<int>(Player::kWhite)]) {
    output += "K";
    castling = true;
  }
  if (castle_queen_[static_cast<int>(Player::kWhite)]) {
    output += "Q";
    castling = true;
  }
  if (castle_king_[static_cast<int>(Player::kBlack)]) {
    output += "k";
    castling = true;
  }
  if (castle_queen_[static_cast<int>(Player::kBlack)]) {
    output += "q";
    castling = true;
  }

  if (!castling) output += "-";

  output += " ";

  // en passant

  if (double_push_pawn_.x >= 0 && double_push_pawn_.y >= 0)
    output +=
        alphabet[double_push_pawn_.x] + std::to_string(double_push_pawn_.y + 1);
  else
    output += "-";

  output += " ";

  // half turns
  output += std::to_string(no_progress_count_) + " ";

  // turns
  int move_count = static_cast<int>(move_count_ / 2) + 1;
  output += std::to_string(move_count);

  return output;
}

State::StatePtr State::FromFEN(std::string fen) {
  std::vector<std::string> parts;

  boost::split(parts, fen, boost::is_any_of(" "));

  if (parts.size() != 6) return nullptr;

  // Board configuration

  std::vector<Piece> pieces;

  // Counts subsequent numbers to allow for numbers with multiple digits
  int number = 0;
  int height = 1;
  for (auto& c : parts.at(0)) {
    if (isdigit(c)) {
      // Copy c to buffer because otherwise atoi will read directly on string.
      char num[2]{c, 0};
      number = 10 * number + std::atoi(num);
      continue;
    }

    if (number > 0) {
      for (int i = 0; i < number; ++i) pieces.push_back(kEmptyPiece);
      number = 0;
    }

    if (c == '/') {
      ++height;
      continue;
    }

    Figure fig;

    switch (std::tolower(c)) {
      case 'k':
        fig = Figure::kKing;
        break;
      case 'q':
        fig = Figure::kQueen;
        break;
      case 'r':
        fig = Figure::kRook;
        break;
      case 'n':
        fig = Figure::kKnight;
        break;
      case 'b':
        fig = Figure::kBishop;
        break;
      case 'p':
        fig = Figure::kPawn;
        break;
      default:
        return nullptr;
    }

    Player player = std::islower(c) ? Player::kBlack : Player::kWhite;
    pieces.push_back(make_piece(fig, player));
  }

  if (number > 0) {
    for (int i = 0; i < number; ++i) pieces.push_back(kEmptyPiece);
  }

  int width = pieces.size() / height;

  if (width * height != pieces.size()) return nullptr;

  StatePtr state =
      std::make_shared<State>(width, height, static_cast<int>(Figure::kCount));
  Board& board = state->GetBoard();

  for (int i = 0; i < pieces.size(); ++i) {
    board.SetField(i % width, height - 1 - static_cast<int>(i / width),
                   pieces.at(i));
  }

  // Player

  if (parts.at(1) == "w")
    state->SetPlayer(Player::kWhite);
  else if (parts.at(1) == "b")
    state->SetPlayer(Player::kBlack);
  else
    return nullptr;

  // Castling

  if (parts.at(2).find("K") == std::string::npos)
    state->SetCastleKing(Player::kWhite);
  if (parts.at(2).find("Q") == std::string::npos)
    state->SetCastleQueen(Player::kWhite);
  if (parts.at(2).find("k") == std::string::npos)
    state->SetCastleKing(Player::kBlack);
  if (parts.at(2).find("q") == std::string::npos)
    state->SetCastleQueen(Player::kBlack);

  // en passant

  if (parts.at(3) != "-") {
    auto itr =
        std::find(alphabet, alphabet + sizeof(alphabet) / sizeof(alphabet[0]),
                  std::tolower(parts.at(3).at(0)));

    if (itr == std::end(alphabet)) return nullptr;

    int x = std::distance(alphabet, itr);
    int y = atoi(&parts.at(3).at(1)) - 1;

    if (x >= board.GetWidth() || y >= board.GetHeight() || x < 0 || y < 0)
      return nullptr;

    state->SetDPushPawnX(std::distance(alphabet, itr));
    state->SetDPushPawnY(atoi(&parts.at(3).at(1)) - 1);
  }

  // half turns

  int half_turns = atoi(&parts.at(4).at(0));

  state->SetNoProgressCount(half_turns);

  // turns

  int turns = (atoi(&parts.at(5).at(0)) - 1) * 2;

  if (state->GetPlayer() == Player::kBlack) turns += 1;

  state->SetMoveCount(turns);

  return state;
}

}  // namespace chess
}  // namespace aithena
