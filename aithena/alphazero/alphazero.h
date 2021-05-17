/**
 * Copyright (C) 2020 - All Right Reserved
 */

#ifndef AITHENA_ALPHAZERO_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_ALPHAZERO_H_

#include <torch/torch.h>
#include <memory>
#include <vector>

#include "alphazero/move.h"
#include "alphazero/node.h"
#include "chess/game.h"

namespace aithena {

class AlphaZeroNet : torch::nn::Module {
 public:
  using NetPtr = std::shared_ptr<AlphaZeroNet>;

  AlphaZeroNet(int board_width, int board_height);
  torch::Tensor forward(torch::Tensor);

 private:
  torch::nn::Conv2d conv{nullptr};
};

class AlphaZero {
 public:
  explicit AlphaZero(chess::Game::GamePtr game);

  // Creates the alphazero interface. game is a pointer to a chess game instance
  // encoding the rules of the game. net is the neural network to be used.
  // time_steps gives the number of time steps to consider for the neural net.
  AlphaZero(chess::Game::GamePtr game, AlphaZeroNet::NetPtr net);

  chess::State::StatePtr DrawAction(chess::State::StatePtr);

 private:
  chess::Game::GamePtr game_;
  AlphaZeroNet::NetPtr network_;
  int time_steps_{8};

  // Generates the neural network input tensor given a AZNode.
  torch::Tensor EncodeNode(AZNode::AZNodePtr);

  // Generates the tensor for a AZNode's state.
  torch::Tensor EncodeNodeState(AZNode::AZNodePtr);

  // Returns the node value selected from the output tensor of the neural net.
  double DecodeNodeValue(torch::Tensor, AZNode::AZNodePtr);
};

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO_H_
