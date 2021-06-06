/**
 * Copyright (C) 2020 - All Right Reserved
 */

#ifndef AITHENA_ALPHAZERO_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_ALPHAZERO_H_

#include <torch/torch.h>

#include <memory>
#include <random>
#include <tuple>
#include <vector>

#include "alphazero/nn.h"
#include "alphazero/node.h"
#include "chess/game.h"
#include "util/dirichlet.h"

namespace aithena {

class ReplayMemory {
 public:
  using Sample = std::tuple<torch::Tensor, std::tuple<torch::Tensor, int>>;

  // Size specifies the maximum number of samples to be stored. A value smaller
  // than one specifies that infinitely many samples can be stored.
  ReplayMemory(int min_size = 0, int max_size = 0);

  int GetSampleCount();
  bool IsReady();

  int GetMinSize();
  int GetMaxSize();
  void SetMinSize(int);
  void SetMaxSize(int);

  // NN input, action values, state value
  void AddSample(torch::Tensor, torch::Tensor, int);
  void AddSample(Sample);

  Sample GetSample();

 private:
  int min_size_;
  int max_size_;
  std::vector<Sample> samples_;

  std::default_random_engine random_generator_;

  int GetRandomSampleIndex();
};

class AlphaZero {
 public:
  // Creates the alphazero interface. game is a pointer to a chess game instance
  // encoding the rules of the game. net is the neural network to be used.
  // time_steps gives the number of time steps to consider for the neural net.
  explicit AlphaZero(chess::Game::GamePtr game, AlphaZeroNet net = nullptr, std::shared_ptr<ReplayMemory> = nullptr);

  chess::State::StatePtr DrawAction(chess::State::StatePtr);
  AZNode::AZNodePtr DrawAction(AZNode::AZNodePtr);

  // Returns whether the network was updated
  bool SelfPlay(chess::State::StatePtr start = nullptr);
  void Simulate(AZNode::AZNodePtr);
  void TrainNetwork();

  // Evaluate the given state
  double EvaluateState(chess::State::StatePtr);
  // Evaluate the given state from the perspective of the given player.
  double EvaluateState(chess::State::StatePtr, chess::Player);

  // Initializes a node by expanding it and setting all children's prior probabilities.
  void Initialize(AZNode::AZNodePtr);

  void SetSimulations(int);
  void SetBatchSize(int);
  void SetSelectPolicy(AZNode::AZNodePtr (*select_policy)(AZNode::AZNodePtr));
  void SetBackpass(void (*backpass)(AZNode::AZNodePtr, double));

  static AZNode::AZNodePtr PUCTSelect(AZNode::AZNodePtr);
  static double PUCTValue(AZNode::AZNodePtr);

  static void AlphaZeroBackpass(AZNode::AZNodePtr, double);

  std::shared_ptr<ReplayMemory> GetReplayMemory();
  AlphaZeroNet GetNetwork();

  static const int kDefaultSimulations = 800;
  static const int kDefaultBatchSize = 4096;

  BenchmarkSet benchmark_;

 private:
  std::shared_ptr<ReplayMemory> replay_memory_{nullptr};
  chess::Game::GamePtr game_{nullptr};
  AlphaZeroNet network_{nullptr};
  int time_steps_{8};
  std::mt19937 random_generator_;
  dirichlet_distribution<std::mt19937> dirichlet_noise_{{.3}};

  // Simulation settings

  int simulations_{kDefaultSimulations};
  int batch_size_{kDefaultBatchSize};
  AZNode::AZNodePtr (*select_policy_)(AZNode::AZNodePtr) = PUCTSelect;
  void (*backpass_)(AZNode::AZNodePtr, double) = AlphaZeroBackpass;
};  // namespace aithena

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO_H_
