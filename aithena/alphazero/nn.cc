/**
 * Copyright (C) 2020 All Rights Reserved
 */

#include "alphazero/nn.h"

#include <algorithm>
#include <tuple>

#include "board/board_plane.h"
#include "chess/game.h"

namespace aithena {

AlphaZeroNetImpl::AlphaZeroNetImpl(chess::Game::GamePtr game) {
  conv_ = torch::nn::Conv2d(torch::nn::Conv2dOptions(kInputSize, 256, 3)
                                .stride(1)
                                .padding(1)
                                .padding_mode(torch::kZeros));
  register_module("conv", conv_);

  torch::nn::Conv2dOptions policy_head_l1_options =
      torch::nn::Conv2dOptions(256, 256, 3)
          .stride(1)
          .padding(1)
          .padding_mode(torch::kZeros);
  torch::nn::Conv2dOptions policy_head_l2_options =
      torch::nn::Conv2dOptions(256, GetNNOutputSize(game), 3)
          .stride(1)
          .padding(1)
          .padding_mode(torch::kZeros);

  policy_head_ = torch::nn::Sequential(
      // Layer 1 (rectified, batch-nomalized convolution)
      torch::nn::Conv2d(policy_head_l1_options), torch::nn::BatchNorm2d(256),
      torch::nn::ReLU(),
      // Layer 2 (final convolution)
      // TODO(*): The final ReLU is not explicitly stated in the alphazero paper
      // but required for computing the loss (ln(x) = undefined for x < 0).
      torch::nn::Conv2d(policy_head_l2_options), torch::nn::ReLU());
  register_module("policy_head", policy_head_);

  torch::nn::Conv2dOptions value_head_l1_options =
      torch::nn::Conv2dOptions(256, 1, 1).stride(1).padding(1).padding_mode(
          torch::kZeros);

  value_head_ = torch::nn::Sequential(
      // Layer 1 (rectified, batch-norm. conv. 1 filter, 1x1 kern., stride 1)
      torch::nn::Conv2d(value_head_l1_options),
      // Layer 2 (rectified linear layer of size 256)
      torch::nn::Flatten(), torch::nn::Linear(GetNNOutputSize(game), 256),
      torch::nn::ReLU(),
      // Layer 3 (tanh linear layer of size 1)
      torch::nn::Linear(256, 1), torch::nn::Tanh());
  register_module("value_head", value_head_);
}

std::tuple<torch::Tensor, torch::Tensor> AlphaZeroNetImpl::forward(
    torch::Tensor x) {
  torch::Tensor body_output = torch::relu(conv_(x));

  torch::Tensor action_values = policy_head_->forward(body_output);
  torch::Tensor state_value = value_head_->forward(body_output);

  return std::make_tuple(action_values, state_value.reshape({-1}));
}

torch::Tensor GetNNInput(AZNode::AZNodePtr node, int time_steps) {
  chess::State::StatePtr state = node->GetState();
  int width = state->GetBoard().GetWidth();
  int height = state->GetBoard().GetWidth();

  torch::Tensor output = torch::empty({0, width, height});

  // Board history
  int time_steps_left = time_steps;
  while (node != nullptr && time_steps_left > 0) {
    output = torch::cat({output, EncodeNodeState(node)});

    node = node->GetParent();
    --time_steps_left;
  }

  // Add remaining planes (one for each piece and two for a one-hot coding of
  // the number of repetitions)
  torch::Tensor zeros =
      torch::zeros({time_steps_left * (plane_count + 2), width, height});
  output = torch::cat({output, zeros}, 0);

  // Player colour
  if (state->GetPlayer() == chess::Player::kWhite)
    output = torch::cat({output, torch::zeros({1, width, height})}, 0);
  else
    output = torch::cat({output, torch::ones({1, width, height})}, 0);

  // Total move count
  output = torch::cat(
      {output, torch::full({1, width, height}, state->GetMoveCount())}, 0);

  // Castling
  std::array<int, 4> castle_values = {
      static_cast<int>(state->GetCastleKing(chess::Player::kWhite)),
      static_cast<int>(state->GetCastleQueen(chess::Player::kWhite)),
      static_cast<int>(state->GetCastleKing(chess::Player::kBlack)),
      static_cast<int>(state->GetCastleQueen(chess::Player::kBlack)),
  };

  for (int value : castle_values)
    output = torch::cat({output, torch::full({1, width, height}, value)}, 0);

  // No progress count
  output = torch::cat(
      {output, torch::full({1, width, height}, state->GetNoProgressCount())},
      0);

  return output.unsqueeze(0);
}

torch::Tensor EncodeNodeState(AZNode::AZNodePtr node) {
  chess::State::StatePtr state = node->GetState();
  Board board = state->GetBoard();  // Copy board as it may be rotated
  int width = board.GetWidth();
  int height = board.GetHeight();
  auto players = chess::Game::players;

  // Orient board and board planes towards player
  if (state->GetPlayer() == chess::Player::kBlack) {
    board.Rotate();
    std::reverse(players.begin(), players.end());
  }

  torch::Tensor output = torch::empty({0, width, height});
  for (auto player : players) {
    for (auto figure : chess::Game::figures) {
      torch::Tensor bt = torch::zeros({1, width, height});
      BoardPlane &b = board.GetPlane(chess::make_piece(figure, player));

      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          if (!b.get(x, y)) continue;

          bt.index_put_({0, y, x}, 1);
        }
      }

      output = torch::cat({output, bt}, 0);
    }
  }

  int repetitions = node->GetStateRepetitions();

  if (repetitions & 0x1)
    output = torch::cat({output, torch::ones({1, width, height})}, 0);
  else
    output = torch::cat({output, torch::zeros({1, width, height})}, 0);

  if (repetitions & 0x2)
    output = torch::cat({output, torch::ones({1, width, height})}, 0);
  else
    output = torch::cat({output, torch::zeros({1, width, height})}, 0);

  return output;
}

int GetNNOutputSize(chess::Game::GamePtr game) {
  int width = game->GetOption("board_width");
  int height = game->GetOption("board_height");

  return GetNNOutputSize(width, height);
}

int GetNNOutputSize(int width, int height) {
  // (Horizontal queen moves + Vertical queen moves + Diagonal queen moves +
  // 8 knight moves + 9 Underpromotions)
  return ((width - 1) * 2 + (height - 1) * 2 +
          (std::min(width, height) - 1) * 4 + kSpecialPlanes);
}

int GetNNOutputPlane(chess::State::StatePtr state) {
  int selector;  // Return value

  torch::Tensor plane;

  Coord source = state->move_info_->GetFrom();
  Coord target = state->move_info_->GetTo();
  Piece piece = state->GetBoard().GetField(target.x, target.y);

  int dx = target.x - source.x;
  int dy = target.y - source.y;

  // Underpromotion
  if (state->move_info_->IsPromotion() &&
      piece.figure != static_cast<int>(chess::Figure::kQueen)) {
    if (piece.figure == static_cast<int>(chess::Figure::kKnight))
      selector = dx + 1;
    else if (piece.figure == static_cast<int>(chess::Figure::kBishop))
      selector = dx + 4;
    else if (piece.figure == static_cast<int>(chess::Figure::kRook))
      selector = dx + 7;
    else
      assert(false);  // Should never reach here - illegal promotion type

    return selector;
  }

  chess::MoveInfo::Direction direction = state->move_info_->GetDirection();

  // Knight move
  if (direction == chess::MoveInfo::Direction::kSpecial) {
    selector = kUnderpromotionPlanes;

    if (dx > 0) selector += 4;

    if (dy > 0) selector += 2;

    if (dy == -1 || dy == 2) selector += 1;

    return selector;
  }

  // Queen move

  int distance = state->move_info_->GetDistance();
  assert(distance > 0);

  selector = kSpecialPlanes +
             (distance - 1) *
                 (static_cast<int>(chess::MoveInfo::Direction::kCount) - 1) +
             static_cast<int>(direction);

  return selector;
}

torch::Tensor GetNNOutput(AZNode::AZNodePtr node) {
  chess::State::StatePtr state = node->GetState();
  int width = state->GetBoard().GetWidth();
  int height = state->GetBoard().GetHeight();

  torch::Tensor tensor =
      torch::zeros({1, GetNNOutputSize(width, height), width, height});

  if (node->GetChildren().size() == 0) return tensor;

  Coord source = node->GetChildren()[0]->GetState()->move_info_->GetFrom();

  for (auto child : node->GetChildren()) {
    double prior = child->GetVisitCount() / node->GetVisitCount();
    tensor.index_put_(
        {0, GetNNOutputPlane(child->GetState()), source.x, source.y}, prior);
  }

  return tensor;
}

double GetNNOutput(torch::Tensor tensor, AZNode::AZNodePtr node) {
  int selector = GetNNOutputPlane(node->GetState());
  Coord source = node->GetState()->move_info_->GetFrom();

  double value = tensor[0][selector][source.x][source.y].item<double>();

  return value;
}

}  // namespace aithena
