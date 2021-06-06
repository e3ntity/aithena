/**
 * Copyright (C) 2020 All Rights Reserved
 */

#ifndef AITHENA_ALPHAZERO_NN_H_
#define AITHENA_ALPHAZERO_NN_H_

#include <memory>
#include <string>
#include <tuple>

#include "alphazero/node.h"
#include "chess/game.h"

namespace aithena {

struct ResidualBlockImpl : torch::nn::Module {
  ResidualBlockImpl(int index, torch::nn::Conv2dOptions conv_options, bool use_cuda = false);

  torch::Tensor forward(torch::Tensor x);

  torch::nn::Conv2d conv1_{nullptr};
  torch::nn::BatchNorm2d bn1_{nullptr};
  torch::nn::ReLU relu1_{};
  torch::nn::Conv2d conv2_{nullptr};
  torch::nn::BatchNorm2d bn2_{nullptr};
  torch::nn::ReLU relu2_{};

  bool use_cuda_;
};

TORCH_MODULE(ResidualBlock);

struct AlphaZeroNetImpl : torch::nn::Module {
  explicit AlphaZeroNetImpl(chess::Game::GamePtr game, bool use_cuda = false);

  // Returns the action value tensor and a state value double.
  std::tuple<torch::Tensor, torch::Tensor> forward(torch::Tensor, bool keep_device = false);

  torch::nn::Sequential body_{nullptr};
  torch::nn::Sequential policy_head_{nullptr};
  torch::nn::Sequential value_head_{nullptr};

  void Save(std::string path);
  void Load(std::string path);

  static const int kInputSize{119};
  bool use_cuda_;
};

TORCH_MODULE(AlphaZeroNet);

static const int kUnderpromotionPlanes = 9;
static const int kKnightPlanes = 8;
static const int kSpecialPlanes = kKnightPlanes + kUnderpromotionPlanes;
static const int plane_count = (chess::Game::player_count * chess::Game::figure_count);

int GetNNInputSize(chess::Game::GamePtr);
int GetNNOutputSize(chess::Game::GamePtr);
int GetNNOutputSize(int width, int height);
int GetNNOutputPlane(chess::State::StatePtr state);

// Generates the tensor for a AZNode's state from the perspective of some player.
torch::Tensor EncodeNodeState(AZNode::AZNodePtr, chess::Player player);

// Generates the neural network input tensor given a AZNode. time_steps specifies the history length of moves.
torch::Tensor GetNNInput(AZNode::AZNodePtr, int time_steps = 8);

// Returns the node's value selected from the output tensor of the neural net.
double GetNNOutput(torch::Tensor, AZNode::AZNodePtr);

// Returns the tensor expected as output for a given node.
torch::Tensor GetNNOutput(AZNode::AZNodePtr node);

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_NN_H_
