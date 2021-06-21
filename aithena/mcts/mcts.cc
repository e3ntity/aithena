/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <chrono>
#include <cmath>
#include <memory>

namespace aithena {

MCTS::MCTS(chess::Game::GamePtr game) : game_{game} {}

chess::State::StatePtr MCTS::DrawAction(chess::State::StatePtr state) {
  MCTSNode::MCTSNodePtr node = std::make_shared<MCTSNode>(game_, state);

  node = DrawAction(node);

  return node->GetState();
}

MCTSNode::MCTSNodePtr MCTS::DrawAction(MCTSNode::MCTSNodePtr start) {
  for (int i = 0; i < simulations_; ++i) Simulate(start);

  MCTSNode::MCTSNodePtr max_child =
      SelectMax(start, [](MCTSNode::MCTSNodePtr child) { return static_cast<double>(child->GetVisitCount()); });

  return max_child;
}

void MCTS::Simulate(MCTSNode::MCTSNodePtr start) {
  MCTSNode::MCTSNodePtr node = start;

  // Selection

  while (!node->IsLeaf() && !node->IsTerminal()) node = select_policy_(node);

  // Expansion

  node->Expand();
  MCTSNode::MCTSNodePtr leaf = RandomSelect(node);

  // Rollout

  MCTSNode::MCTSNodePtr rollout_node = std::make_shared<MCTSNode>(game_, leaf->GetState());

  while (true) {
    rollout_node->Expand();

    if (rollout_node->IsTerminal()) break;

    rollout_node = rollout_policy_(rollout_node);
  }

  int result = game_->GetStateResult(rollout_node->GetState());

  // Backpass

  backpass_(leaf, -result);
}

MCTSNode::MCTSNodePtr MCTS::SelectMax(MCTSNode::MCTSNodePtr node, double (*evaluate)(MCTSNode::MCTSNodePtr)) {
  int max_value{INT_MIN};
  MCTSNode::MCTSNodePtr max_child{nullptr};

  for (auto child : node->GetChildren()) {
    int value = evaluate(child);

    if (value < max_value) continue;

    if (value == max_value) {
      double random = static_cast<double>(rand()) / RAND_MAX;

      max_child = random > 0.5 ? max_child : child;
    } else {
      max_child = child;
      max_value = value;
    }
  }

  return max_child;
}

MCTSNode::MCTSNodePtr MCTS::UCTSelect(MCTSNode::MCTSNodePtr node) {
  assert(node->GetVisitCount() > 0);

  return SelectMax(node, [](MCTSNode::MCTSNodePtr child) {
    MCTSNode::MCTSNodePtr parent = child->GetParent();

    assert(child->GetVisitCount() > 0);

    double exploitation = child->GetMeanWinCount();
    double exploration =
        sqrt(log(static_cast<double>(parent->GetVisitCount())) / static_cast<double>(child->GetVisitCount()));

    return exploitation + 1.41 * exploration;
  });
}

MCTSNode::MCTSNodePtr MCTS::RandomSelect(MCTSNode::MCTSNodePtr node) {
  return SelectMax(node, [](MCTSNode::MCTSNodePtr child) { return static_cast<double>(rand()) / RAND_MAX; });
}

void MCTS::Backpass(MCTSNode::MCTSNodePtr start, int value) {
  MCTSNode::MCTSNodePtr node = start;

  int i = 0;
  while (node != nullptr) {
    node->Update(i % 2 == 0 ? value : -value);

    node = node->GetParent();
    ++i;
  }
}

void MCTS::SetSimulations(int simulations) { simulations_ = simulations; }

}  // namespace aithena
