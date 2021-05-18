/**
 * Copyright (C) 2020 All Right Reserved
 */

#include "alphazero/node.h"

namespace aithena {

AZNode::AZNode(chess::Game::GamePtr game, chess::State::StatePtr state,
               AZNode::AZNodePtr parent)
    : game_{game}, state_{state}, parent_{parent} {}

chess::State::StatePtr AZNode::GetState() { return state_; }

AZNode::AZNodePtr AZNode::GetParent() { return parent_; }

std::vector<AZNode::AZNodePtr> AZNode::GetChildren() { return children_; }

void AZNode::Expand(torch::Tensor priors) {
  AZNodePtr node;

  auto moves = game_->GetLegalActions(state_);
  for (auto move : moves) {
    node = std::make_shared<AZNode>(game_, move, shared_from_this());
    // TODO(*): select appropriate prior and update node statistics with it
    children_.push_back(node);
  }

  expanded_ = true;
}

bool AZNode::IsExpanded() { return expanded_; }

double AZNode::GetMeanActionValue() {
  if (visit_count_ <= 0) return 0;

  return action_value_ / static_cast<double>(visit_count_);
}

double AZNode::GetPrior() { return prior_; }

double AZNode::GetTotalActionValue() { return action_value_; }

int AZNode::GetVisitCount() { return visit_count_; }

}  // namespace aithena
