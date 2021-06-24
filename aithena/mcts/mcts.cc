/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <float.h>

#include <chrono>
#include <cmath>
#include <memory>

#include "chess/game.h"
#include "chess/util.h"

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

  if (node->IsTerminal()) {
    int result = game_->GetStateResult(node->GetState());
    backpass_(node, -result);

    return;
  }

  MCTSNode::MCTSNodePtr leaf = RandomSelect(node);

  // Rollout

  MCTSNode::MCTSNodePtr rollout_node = std::make_shared<MCTSNode>(game_, leaf->GetState());

  bool negate = true;
  while (true) {
    rollout_node->Expand();

    if (rollout_node->IsTerminal()) break;

    rollout_node = rollout_policy_(rollout_node);
    negate = !negate;
  }

  int result = (negate ? -1 : 1) * game_->GetStateResult(rollout_node->GetState());

  // Backpass

  backpass_(leaf, result);
}

MCTSNode::MCTSNodePtr MCTS::SelectMax(MCTSNode::MCTSNodePtr node, double (*evaluate)(MCTSNode::MCTSNodePtr)) {
  double max_value{-DBL_MAX};
  std::vector<MCTSNode::MCTSNodePtr> max_children;

  for (auto child : node->GetChildren()) {
    double value = evaluate(child);

    if (value < max_value) continue;

    if (value > max_value) max_children.clear();

    max_children.push_back(child);
    max_value = value;
  }

  assert(max_children.size() > 0);  // TODO: remove

  if (max_children.size() == 1) return max_children.at(0);

  int random = rand() % max_children.size();

  return max_children.at(random);
}

MCTSNode::MCTSNodePtr MCTS::UCTSelect(MCTSNode::MCTSNodePtr node) {
  assert(node->GetVisitCount() > 0);

  return SelectMax(node, [](MCTSNode::MCTSNodePtr child) {
    MCTSNode::MCTSNodePtr parent = child->GetParent();

    assert(child->GetVisitCount() > 0);

    double exploitation = child->GetMeanValue();
    double exploration =
        sqrt(log(static_cast<double>(parent->GetVisitCount())) / static_cast<double>(child->GetVisitCount()));

    double value = exploitation + 1.41 * exploration;

    return value;
  });
}

MCTSNode::MCTSNodePtr MCTS::RandomSelect(MCTSNode::MCTSNodePtr node) {
  return SelectMax(node, [](MCTSNode::MCTSNodePtr child) { return static_cast<double>(rand()) / RAND_MAX; });
}

void MCTS::Backpass(MCTSNode::MCTSNodePtr start, int value) {
  MCTSNode::MCTSNodePtr node = start;

  bool negate = false;
  double discount = 1;
  while (node != nullptr) {
    node->Update(discount * static_cast<double>(negate ? -value : value));

    node = node->GetParent();
    negate = !negate;
    discount = discount * 0.99;  // TODO: make discount factor modifiable
  }
}

void MCTS::SetSimulations(int simulations) { simulations_ = simulations; }

}  // namespace aithena
