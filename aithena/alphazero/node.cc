/**
 * Copyright (C) 2020 All Rights Reserved
 */

#include "alphazero/node.h"

#include "alphazero/nn.h"

namespace aithena {

AZNode::AZNode(chess::Game::GamePtr game, chess::State::StatePtr state, AZNode::AZNodePtr parent) : game_{game} {
  state_ = state;
  parent_ = parent;
}

chess::State::StatePtr AZNode::GetState() { return state_; }

AZNode::AZNodePtr AZNode::GetParent() { return parent_; }

std::vector<AZNode::AZNodePtr> AZNode::GetChildren() { return children_; }

int AZNode::GetStateRepetitions() {
  int repetitions = 0;

  AZNode::AZNodePtr current_node = GetParent();
  while (current_node != nullptr) {
    if (*current_node->GetState() == *GetState()) ++repetitions;

    current_node = current_node->GetParent();
  }

  return repetitions;
}

void AZNode::SetPrior(double prior) { prior_ = prior; }

void AZNode::Update(double value) {
  action_value_ += value;
  ++visit_count_;
}

void AZNode::Expand() {
  if (expanded_) return;

  AZNodePtr node;

  auto moves = game_->GetLegalActions(state_);
  for (auto move : moves) {
    node = std::make_shared<AZNode>(game_, move, shared_from_this());

    children_.push_back(node);
  }

  expanded_ = true;
}

bool AZNode::IsExpanded() { return expanded_; }

bool AZNode::IsTerminal() {
  if (IsExpanded()) return children_.size() <= 0;

  return game_->IsTerminalState(GetState());
}

double AZNode::GetMeanActionValue() {
  if (visit_count_ <= 0) return 0;

  return action_value_ / static_cast<double>(visit_count_);
}

double AZNode::GetPrior() { return prior_; }

double AZNode::GetTotalActionValue() { return action_value_; }

int AZNode::GetVisitCount() { return visit_count_; }

}  // namespace aithena
