/**
 * Copyright (C) 2020 All Rights Reserved
 */

#include "alphazero/nn.h"

#include <algorithm>
#include <tuple>

#include "board/board_plane.h"
#include "chess/game.h"
#include "chess/util.h"

namespace aithena {

ResidualBlockImpl::ResidualBlockImpl(int index, torch::nn::Conv2dOptions conv_options, bool use_cuda)
    : conv1_{conv_options},
      conv2_{conv_options},
      bn1_{conv_options.out_channels()},
      bn2_{conv_options.out_channels()},
      use_cuda_{use_cuda} {
  if (use_cuda_) {
    conv1_->to(torch::kCUDA);
    conv2_->to(torch::kCUDA);
    bn1_->to(torch::kCUDA);
    bn2_->to(torch::kCUDA);
    relu1_->to(torch::kCUDA);
    relu2_->to(torch::kCUDA);
  }

  register_module("resblock" + std::to_string(index) + "_conv1", conv1_);
  register_module("resblock" + std::to_string(index) + "_conv2", conv2_);
  register_module("resblock" + std::to_string(index) + "_bn1", bn1_);
  register_module("resblock" + std::to_string(index) + "_bn2", bn2_);
  register_module("resblock" + std::to_string(index) + "_relu1", relu1_);
  register_module("resblock" + std::to_string(index) + "_relu2", relu2_);
}

torch::Tensor ResidualBlockImpl::forward(torch::Tensor x) {
  torch::Tensor output = relu1_(bn1_(conv1_(x)));
  output = relu2_(bn2_(conv2_(output)) + x);

  return x;
}

AlphaZeroNetImpl::AlphaZeroNetImpl(chess::Game::GamePtr game, bool use_cuda) : use_cuda_{use_cuda} {
  int width = game->GetOption("board_width");
  int height = game->GetOption("board_height");

  body_ = torch::nn::Sequential(
      // Layer 1 (rectified, batch-normalized convolution)
      torch::nn::Conv2d(torch::nn::Conv2dOptions(kInputSize, 256, 3).stride(1).padding(1).padding_mode(torch::kZeros)),
      torch::nn::BatchNorm2d(256), torch::nn::ReLU());

  torch::nn::Conv2dOptions body_options =
      torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1).padding_mode(torch::kZeros);

  for (int i = 0; i < 19; ++i) body_->push_back(ResidualBlock(i, body_options, use_cuda_));

  register_module("body", body_);

  torch::nn::Conv2dOptions policy_head_l1_options =
      torch::nn::Conv2dOptions(256, 256, 3).stride(1).padding(1).padding_mode(torch::kZeros);
  torch::nn::Conv2dOptions policy_head_l2_options =
      torch::nn::Conv2dOptions(256, GetNNOutputSize(game), 3).stride(1).padding(1).padding_mode(torch::kZeros);

  policy_head_ = torch::nn::Sequential(
      // Layer 1 (rectified, batch-nomalized convolution)
      torch::nn::Conv2d(policy_head_l1_options), torch::nn::BatchNorm2d(256), torch::nn::ReLU(),
      // Layer 2 (final convolution)
      // TODO(*): The final ReLU is not explicitly stated in the alphazero paper
      // but required for computing the loss (ln(x) = undefined for x < 0).
      torch::nn::Conv2d(policy_head_l2_options), torch::nn::ReLU());
  register_module("policy_head", policy_head_);

  torch::nn::Conv2dOptions value_head_l1_options =
      torch::nn::Conv2dOptions(256, 1, 1).stride(1).padding(1).padding_mode(torch::kZeros);

  value_head_ = torch::nn::Sequential(
      // Layer 1 (rectified, batch-norm. conv. 1 filter, 1x1 kern., stride 1)
      torch::nn::Conv2d(value_head_l1_options), torch::nn::BatchNorm2d(1), torch::nn::ReLU(),
      // Layer 2 (rectified linear layer of size 256)
      torch::nn::Flatten(), torch::nn::Linear((width + 2) * (height + 2), 256), torch::nn::ReLU(),
      // Layer 3 (tanh linear layer of size 1)
      torch::nn::Linear(256, 1), torch::nn::Tanh());
  register_module("value_head", value_head_);

  if (use_cuda_) {
    body_->to(torch::kCUDA);
    policy_head_->to(torch::kCUDA);
    value_head_->to(torch::kCUDA);
  }
}

std::tuple<torch::Tensor, torch::Tensor> AlphaZeroNetImpl::forward(torch::Tensor x, bool keep_device) {
  if (use_cuda_) x = x.to(torch::kCUDA);

  torch::Tensor body_output = body_->forward(x);

  torch::Tensor action_values = policy_head_->forward(body_output);
  torch::Tensor state_value = value_head_->forward(body_output);

  if (!keep_device && use_cuda_) {
    action_values = action_values.to(torch::kCPU);
    state_value = state_value.to(torch::kCPU);
  }

  return std::make_tuple(action_values, state_value.reshape({-1}));
}

void AlphaZeroNetImpl::Save(std::string path) {
  torch::save(body_, path + "-body.pt");
  torch::save(policy_head_, path + "-policy_head.pt");
  torch::save(value_head_, path + "-value_head.pt");
}

void AlphaZeroNetImpl::Load(std::string path) {
  torch::load(body_, path + "-body.pt");
  torch::load(policy_head_, path + "-policy_head.pt");
  torch::load(value_head_, path + "-value_head.pt");
}

torch::Tensor GetNNInput(AZNode::AZNodePtr node, int time_steps) {
  chess::State::StatePtr state = node->GetState();
  int width = state->GetBoard().GetWidth();
  int height = state->GetBoard().GetWidth();

  torch::Tensor output = torch::empty({0, width, height});

  // Board history
  int time_steps_left = time_steps;
  while (node != nullptr && time_steps_left > 0) {
    output = torch::cat({output, EncodeNodeState(node, state->GetPlayer())});

    node = node->GetParent();
    --time_steps_left;
  }

  // Add remaining planes (one for each piece and two for a one-hot coding of
  // the number of repetitions)
  torch::Tensor zeros = torch::zeros({time_steps_left * (plane_count + 2), width, height});
  output = torch::cat({output, zeros}, 0);

  // Player colour
  if (state->GetPlayer() == chess::Player::kWhite)
    output = torch::cat({output, torch::zeros({1, width, height})}, 0);
  else
    output = torch::cat({output, torch::ones({1, width, height})}, 0);

  // Total move count
  output = torch::cat({output, torch::full({1, width, height}, state->GetMoveCount())}, 0);

  // Castling
  std::array<int, 4> castle_values = {
      static_cast<int>(state->GetCastleKing(chess::Player::kWhite)),
      static_cast<int>(state->GetCastleQueen(chess::Player::kWhite)),
      static_cast<int>(state->GetCastleKing(chess::Player::kBlack)),
      static_cast<int>(state->GetCastleQueen(chess::Player::kBlack)),
  };

  for (int value : castle_values) output = torch::cat({output, torch::full({1, width, height}, value)}, 0);

  // No progress count
  output = torch::cat({output, torch::full({1, width, height}, state->GetNoProgressCount())}, 0);

  return output.unsqueeze(0);
}

torch::Tensor EncodeNodeState(AZNode::AZNodePtr node, chess::Player player) {
  chess::State::StatePtr state = node->GetState();
  Board board = state->GetBoard();  // Copy board as it may be rotated
  int width = board.GetWidth();
  int height = board.GetHeight();

  // Order planes so that the current player is the first player.
  std::array<chess::Player, 2> players = {player, chess::GetOpponent(player)};

  torch::Tensor output = torch::empty({0, width, height});
  for (auto player : players) {
    for (auto figure : chess::Game::figures) {
      torch::Tensor bt = torch::zeros({1, width, height});
      BoardPlane &b = board.GetPlane(chess::make_piece(figure, player));

      for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
          if (!b.get(x, y)) continue;

          bt.index_put_({0, x, y}, 1);
        }
      }

      output = torch::cat({output, bt}, 0);
    }
  }

  // Rotate board and board planes towards current player
  if (player == chess::Player::kBlack) output = output.rot90(1, {1, 2}).rot90(1, {1, 2});

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
  return ((width - 1) * 2 + (height - 1) * 2 + (std::min(width, height) - 1) * 4 + kSpecialPlanes);
}

int GetNNOutputPlane(chess::State::StatePtr state) {
  int selector;  // Return value

  torch::Tensor plane;

  Coord source = state->move_info_->GetFrom();
  Coord target = state->move_info_->GetTo();
  Piece piece = state->GetBoard().GetField(target.x, target.y);

  int dx = target.x - source.x;
  int dy = target.y - source.y;

  // Rotate towards player that made the move (If it is white's turn, black made the move)
  if (state->GetPlayer() == chess::Player::kWhite) {
    dx = state->GetBoard().GetWidth() - dx - 1;
    dy = state->GetBoard().GetHeight() - dy - 1;
  }

  // Underpromotion
  if (state->move_info_->IsPromotion() && piece.figure != static_cast<int>(chess::Figure::kQueen)) {
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

  // Rotate towards player that made the move (If it is white's turn, black made the move)
  if (state->GetPlayer() == chess::Player::kWhite) direction = chess::MoveInfo::GetOppositeDirection(direction);

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

  selector = kSpecialPlanes + (distance - 1) * (static_cast<int>(chess::MoveInfo::Direction::kCount) - 1) +
             static_cast<int>(direction);

  return selector;
}

torch::Tensor GetNNOutput(AZNode::AZNodePtr node) {
  chess::State::StatePtr state = node->GetState();
  int width = state->GetBoard().GetWidth();
  int height = state->GetBoard().GetHeight();

  torch::Tensor tensor = torch::zeros({1, GetNNOutputSize(width, height), width, height});

  if (node->GetChildren().size() == 0) return tensor;

  for (auto child : node->GetChildren()) {
    Coord source = child->GetState()->move_info_->GetFrom();
    double prior = static_cast<double>(child->GetVisitCount()) / static_cast<double>(node->GetVisitCount());

    tensor.index_put_({0, GetNNOutputPlane(child->GetState()), source.x, source.y}, prior);
  }

  // Rotate board towards player
  if (state->GetPlayer() == chess::Player::kBlack) tensor = tensor.rot90(1, {2, 3}).rot90(1, {2, 3});

  return tensor;
}

double GetNNOutput(torch::Tensor tensor, AZNode::AZNodePtr node) {
  int selector = GetNNOutputPlane(node->GetState());
  Coord source = node->GetState()->move_info_->GetFrom();

  torch::Tensor action_tensor = tensor[0][selector];

  // Rotate board towards player (if node's player is white, the move was made by black -> so rotate)
  if (node->GetState()->GetPlayer() == chess::Player::kWhite)
    action_tensor = action_tensor.rot90(1, {0, 1}).rot90(1, {0, 1});

  double value = action_tensor[source.x][source.y].item<double>();

  return value;
}

}  // namespace aithena
