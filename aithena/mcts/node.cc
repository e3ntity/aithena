/*
Copyright 2020 All rights reserved.
*/

#include "mcts/node.h"

#include <memory>

#include "chess/game.h"

namespace aithena {

// Constructors

MCTSNode::MCTSNode(chess::Game::GamePtr game, chess::State::StatePtr state, MCTSNode::MCTSNodePtr parent)
    : game_{game}, state_{state}, parent_{parent} {}

chess::State::StatePtr MCTSNode::GetState() { return state_; }
MCTSNode::MCTSNodePtr MCTSNode::GetParent() { return parent_.lock(); }
std::vector<MCTSNode::MCTSNodePtr> MCTSNode::GetChildren() { return children_; }
void MCTSNode::SetParent(MCTSNode::MCTSNodePtr parent) { parent_ = parent; }

void MCTSNode::Update(double value) {
  total_value_ += value;
  visit_count_ += 1;
}

void MCTSNode::Expand() {
  if (IsExpanded()) return;

  MCTSNode::MCTSNodePtr node;

  auto moves = game_->GetLegalActions(state_);
  for (auto move : moves) {
    node = std::make_shared<MCTSNode>(game_, move, shared_from_this());

    children_.push_back(node);
  }

  expanded_ = true;
}

bool MCTSNode::IsExpanded() { return expanded_; }

bool MCTSNode::IsLeaf() {
  if (!IsExpanded()) return true;

  for (auto child : children_) {
    if (child->GetVisitCount() <= 0) return true;
  }

  return false;
}

bool MCTSNode::IsTerminal() {
  if (IsExpanded()) return children_.size() <= 0;

  return game_->IsTerminalState(GetState());
}

double MCTSNode::GetMeanValue() {
  if (visit_count_ == 0) return 0;

  return total_value_ / static_cast<double>(visit_count_);
}
double MCTSNode::GetTotalValue() { return total_value_; }
int MCTSNode::GetVisitCount() { return visit_count_; }

}  // namespace aithena
