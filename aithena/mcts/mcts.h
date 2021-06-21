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
class MCTS {
 public:
  MCTS(chess::Game::GamePtr game);

  chess::State::StatePtr DrawAction(chess::State::StatePtr);
  MCTSNode::MCTSNodePtr DrawAction(MCTSNode::MCTSNodePtr);

  void Simulate(MCTSNode::MCTSNodePtr);

  // Selects the maximum child according to some evaluation function.
  static MCTSNode::MCTSNodePtr SelectMax(MCTSNode::MCTSNodePtr node, double (*evaluate)(MCTSNode::MCTSNodePtr));

  // Selects child according to UCT.
  static MCTSNode::MCTSNodePtr UCTSelect(MCTSNode::MCTSNodePtr);
  // Selects a random child.
  static MCTSNode::MCTSNodePtr RandomSelect(MCTSNode::MCTSNodePtr);
  // Selects child with highest visit count.
  static MCTSNode::MCTSNodePtr VisitSelect(MCTSNode::MCTSNodePtr);

  static void Backpass(MCTSNode::MCTSNodePtr, int);

  void SetSimulations(int);

  const static int kDefaultSimulations = 1000;

 private:
  chess::Game::GamePtr game_;

  int simulations_{kDefaultSimulations};

  MCTSNode::MCTSNodePtr (*select_policy_)(MCTSNode::MCTSNodePtr) = UCTSelect;
  MCTSNode::MCTSNodePtr (*rollout_policy_)(MCTSNode::MCTSNodePtr) = RandomSelect;
  void (*backpass_)(MCTSNode::MCTSNodePtr, int) = Backpass;
};

}  // namespace aithena

#endif  // AITHENA_MCTS_MCTS_H_
