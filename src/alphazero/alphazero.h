#ifndef AITHENA_ALPHAZERO_ALPHAZERO
#define AITHENA_ALPHAZERO_ALPHAZERO

#include "alphazero/neural_network.h"
#include "alphazero/node.h"

#include <chrono>

namespace aithena {
namespace alphazero {

class AlphaZero {
 public:
  using ReplayMemory = std::vector<std::tuple<AZNode::NodePtr, int>>;

  AlphaZero(std::shared_ptr<chess::Game>, AZNeuralNetwork);

  // Runs a single simulation for alphazero starting from a root node.
  void Run(AZNode::NodePtr);

  // Runs a training game against itself and returns the final node.
  AZNode::NodePtr SelfPlayGame(AZNode::NodePtr, int simulations = 800);

  // Trains the network for a single self play game. Returns whether the NN was
  // updates or if the sample count was too small for batch size.
  bool Train(chess::Game::GameState, std::shared_ptr<ReplayMemory>, int, int);

  void Backpropagate(AZNode::NodePtr, int);
  AZNode::NodePtr Select(AZNode::NodePtr node,
                         AZNode::NodePtr (*next)(AZNode::NodePtr));
  static AZNode::NodePtr PUCTSelect(AZNode::NodePtr);

  // Returns the average time per select in seconds.
  double BenchmarkSelect();
  // Returns the average time per backpropagation in seconds.
  double BenchmarkBackprop();
  // Returns the average time per run in seconds.
  double BenchmarkRun();

  void Save(std::string path);
  void Load(std::string path);

 private:
  double Benchmark(std::vector<std::chrono::milliseconds>);

  std::shared_ptr<chess::Game> game_;
  AZNeuralNetwork nn_;

  // Stats
  std::vector<std::chrono::milliseconds> time_select_{};
  std::vector<std::chrono::milliseconds> time_backprop_{};
  std::vector<std::chrono::milliseconds> time_run_{};
};

}  // alphazero
}  // aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO
