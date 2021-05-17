/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_MCTS_MCTS_H_
#define AITHENA_MCTS_MCTS_H_

#include <chrono>

#include "benchmark/benchmark.h"
#include "mcts/node.h"

namespace aithena {

// Monte Carlo Tree Search
template <typename Game>
class MCTS {
 public:
  using Node = MCTSNode<Game>;

  MCTS(std::shared_ptr<Game>);

  // Runs the MCTS algorithm with a certain number of simulations each round,
  // starting at the given node.
  void Run(typename Node::NodePtr, int simulations = 1);

  // MCTS Selection, starting at some node and selecting successive children
  // according to the search strategy until a leaf node / terminal node is
  // reached.
  typename Node::NodePtr Select(
      typename Node::NodePtr,
      typename Node::NodePtr (*next)(typename Node::NodePtr));

  // Simulates a playout using some selection strategy, starting from some node.
  // Returns the result of the playout from the perspective of the node from
  // which it was started.
  int Simulate(typename Node::NodePtr,
               typename Node::NodePtr (*next)(typename Node::NodePtr));

  // Backpropagates the result of a playout from the node where it was started
  // to the root.
  void Backpropagate(typename Node::NodePtr, int);

  // Strategies for selecting the next node.

  // Selects a random next node. Make sure to previously seed the prng with
  // srand(). Assumes that the node has already been expanded.
  static typename Node::NodePtr RandomSelect(typename Node::NodePtr);

  // Selects the next node according to the upper confidence bound.
  // Assumes that the node has already been expanded and that each child node
  // has been visited at least once.
  static typename Node::NodePtr UCTSelect(typename Node::NodePtr);

  // Selects the next node greedily.
  static typename Node::NodePtr GreedySelect(typename Node::NodePtr);

  // Returns the average time per select in seconds.
  double BenchmarkSelect();
  // Returns the average time per simulation in seconds.
  double BenchmarkSimulate();
  // Returns the average time per backpropagation in seconds.
  double BenchmarkBackpropagate();
  double BenchmarkRun();

  // Benchmark set. Benchmarks are named after the functions. e.g. "Run"
  BenchmarkSet bm_;

 protected:
  std::shared_ptr<Game> game_;
};

template class MCTS<::aithena::chess::Game>;

}  // namespace aithena

#endif  // AITHENA_MCTS_MCTS_H_