/**
 * Copyright (C) 2020 - All Right Reserved
 */

#include "alphazero/alphazero.h"

#include <torch/torch.h>

#include "alphazero/move.h"

namespace aithena {

AlphaZeroNet::AlphaZeroNet(int board_width, int board_height) {
  int output_size = AZMaxChessMoveValue(board_width, board_height);

  std::cout << output_size << std::endl;

  conv = register_module(
      "conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(119, output_size, 3)
                                    .stride(1)
                                    .padding(1)
                                    .padding_mode(torch::kZeros)));
}

torch::Tensor AlphaZeroNet::forward(torch::Tensor x) {
  torch::Tensor val = conv(x);

  return val;
}

static const int plane_count =
    (chess::Game::player_count * chess::Game::figure_count);

AlphaZero::AlphaZero(chess::Game::GamePtr game) : AlphaZero(game, nullptr) {
  int width = game_->GetOption("board_width");
  int height = game_->GetOption("board_height");

  network_ = std::make_shared<AlphaZeroNet>(width, height);
}

AlphaZero::AlphaZero(chess::Game::GamePtr game, AlphaZeroNet::NetPtr net)
    : game_{game}, network_{net} {}

chess::State::StatePtr AlphaZero::DrawAction(chess::State::StatePtr state) {
  AZNode::AZNodePtr node = std::make_shared<AZNode>(game_, state);

  torch::Tensor input = EncodeNode(node).unsqueeze(0);
  torch::Tensor output = network_->forward(input);

  for (auto child : game_->GetLegalActions(*node->GetState())) {
    double value = DecodeNodeValue(output, node);

    std::cout << value << std::endl;
  }

  return state;
}

torch::Tensor AlphaZero::EncodeNode(AZNode::AZNodePtr node) {
  chess::State::StatePtr state = node->GetState();
  int width = state->GetBoard().GetWidth();
  int height = state->GetBoard().GetWidth();

  torch::Tensor output = torch::empty({0, width, height});

  // Board history
  int time_steps_left = time_steps_;
  while (node != nullptr) {
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

  return output;
}

torch::Tensor AlphaZero::EncodeNodeState(AZNode::AZNodePtr node) {
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
      BoardPlane& b = board.GetPlane(chess::make_piece(figure, player));

      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          if (!b.get(x, y)) continue;

          bt.index_put_({0, y, x}, 1);
        }
      }

      output = torch::cat({output, bt}, 0);
    }
  }

  int repetitions = 0;

  // Append one-hot coded number of board configuration repetitions.

  AZNode::AZNodePtr current_node = node->GetParent();
  while (current_node != nullptr) {
    if (current_node->GetState() == node->GetState()) ++repetitions;

    current_node = current_node->GetParent();
  }

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

double AlphaZero::DecodeNodeValue(torch::Tensor tensor,
                                  AZNode::AZNodePtr node) {
  chess::State::StatePtr state = node->GetState();

  return 0;
}

}  // namespace aithena
