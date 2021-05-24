/**
 * Copyright (C) 2020 All Rights Reserved
 */

#ifndef AITHENA_ALPHAZERO_NN_H_
#define AITHENA_ALPHAZERO_NN_H_

#include <memory>
#include <tuple>

#include "alphazero/node.h"
#include "chess/game.h"

namespace aithena {

struct AlphaZeroNetImpl : torch::nn::Module {
  explicit AlphaZeroNetImpl(chess::Game::GamePtr game);

  // Returns the action value tensor and a state value double.
  std::tuple<torch::Tensor, torch::Tensor> forward(torch::Tensor);

  torch::nn::Conv2d conv_{nullptr};

  torch::nn::Sequential policy_head_{nullptr};
  torch::nn::Sequential value_head_{nullptr};

  static const int kInputSize{119};
};

TORCH_MODULE(AlphaZeroNet);

static const int kUnderpromotionPlanes = 9;
static const int kKnightPlanes = 8;
static const int kSpecialPlanes = kKnightPlanes + kUnderpromotionPlanes;
static const int plane_count =
    (chess::Game::player_count * chess::Game::figure_count);

int GetNNInputSize(chess::Game::GamePtr);
int GetNNOutputSize(chess::Game::GamePtr);
int GetNNOutputSize(int width, int height);

// Generates the tensor for a AZNode's state.
torch::Tensor EncodeNodeState(AZNode::AZNodePtr);

// Generates the neural network input tensor given a AZNode.
torch::Tensor GetNNInput(AZNode::AZNodePtr, int time_steps = 8);

// Returns the node value selected from the output tensor of the neural net.
double GetNNOutput(torch::Tensor, AZNode::AZNodePtr);

// Returns the tensor expected as output for a given node.
torch::Tensor GetNNOutput(AZNode::AZNodePtr node);

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_NN_H_
