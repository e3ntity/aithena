/**
 * Copyright (C) 2020 - All Right Reserved
 */

#ifndef AITHENA_ALPHAZERO_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_ALPHAZERO_H_

#ifndef AITHENA_DISABLE_CUDA_UPDATES
#define AITHENA_DISABLE_CUDA_UPDATES false
#endif

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
  using Sample = std::tuple<torch::Tensor, std::tuple<torch::Tensor, double>>;

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
  void AddSample(torch::Tensor, torch::Tensor, double);
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

  // Returns the state that is the result of the action estimated to be the best for the current player.
  chess::State::StatePtr DrawAction(chess::State::StatePtr);
  // Runs the algorithm on the given node and returns the best node, keeping all statistics that were built up during
  // simulation. The node passed as argument must not have a parent node!
  AZNode::AZNodePtr DrawAction(AZNode::AZNodePtr);

  // Plays a game against itself and stores the result in the replay memory. Returns whether the network was updated.
  void SelfPlay(chess::State::StatePtr start = nullptr);

  // Samples a batch of samples from the replay memory and updates the neural network with it.
  void TrainNetwork();

  // Runs a simulation, starting from the given node and backpasses the result.
  void Simulate(AZNode::AZNodePtr);

  // Evaluate the given state
  double EvaluateState(chess::State::StatePtr);
  // Evaluate the given state from the perspective of the given player.
  double EvaluateState(chess::State::StatePtr, chess::Player);

  // Initializes a node by expanding it and setting all children's prior probabilities.
  void Initialize(AZNode::AZNodePtr);

  void SetSimulations(int);
  void SetBatchSize(int);
  void SetUseCUDA(bool);
  void SetDirichletNoiseAlpha(double);
  void SetSelectPolicy(AZNode::AZNodePtr (*select_policy)(AZNode::AZNodePtr));
  void SetBackpass(void (*backpass)(AZNode::AZNodePtr, double));

  static AZNode::AZNodePtr PUCTSelect(AZNode::AZNodePtr);
  static double PUCTValue(AZNode::AZNodePtr);

  static void AlphaZeroBackpass(AZNode::AZNodePtr, double);

  std::shared_ptr<ReplayMemory> GetReplayMemory();
  AlphaZeroNet GetNetwork();

  static const int kDefaultSimulations = 800;
  static const int kDefaultBatchSize = 4096;
  static constexpr double kDefaultDiscountFactor = 0.95;
  static constexpr double kDefaultDirichletNoiseAlpha = 0.3;

  BenchmarkSet benchmark_;

 private:
  // TODO: this flag disables using the GPU for updating the NN. This is required for my old PC but should normally be
  // disabled or completely removed from the code!
  bool disable_cuda_update_{AITHENA_DISABLE_CUDA_UPDATES};

  std::shared_ptr<ReplayMemory> replay_memory_{nullptr};
  chess::Game::GamePtr game_{nullptr};
  AlphaZeroNet network_{nullptr};
  int time_steps_{8};
  std::mt19937 random_generator_;
  dirichlet_distribution<std::mt19937> dirichlet_noise_{{kDefaultDirichletNoiseAlpha}};

  // Simulation settings

  int simulations_{kDefaultSimulations};
  int batch_size_{kDefaultBatchSize};
  double discount_factor_{kDefaultDiscountFactor};
  AZNode::AZNodePtr (*select_policy_)(AZNode::AZNodePtr) = PUCTSelect;
  void (*backpass_)(AZNode::AZNodePtr, double) = AlphaZeroBackpass;
};  // namespace aithena

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO_H_
