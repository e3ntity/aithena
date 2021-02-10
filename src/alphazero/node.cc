/*
Copyright 2020 All rights reserved.
*/

#include "alphazero/node.h"

namespace aithena {
namespace alphazero {

AZNode::AZNode(::aithena::chess::State state, AZNodePtr parent,
               std::shared_ptr<::aithena::chess::Game> game)
    : state_{state}, parent_{parent}, game_{game} {};

void AZNode::Expand() {
  if (IsExpanded()) return;

  auto moves = game_->GetLegalActions(state_);

  if (moves.size() < 1) return;

  for (auto state : moves) {
    children_.push_back(
        std::shared_ptr<AZNode>(new AZNode(state, shared_from_this(), game_)));
  }

  expanded_ = true;
}

bool AZNode::IsExpanded() { return expanded_; }

std::vector<AZNodePtr> AZNode::GetChildren() { return children_; }
AZNodePtr AZNode::GetParent() { return parent_; }
::aithena::chess::State AZNode::GetState() { return state_; }

double AZNode::GetActionValue() { return action_value_; }
double AZNode::GetMeanActionValue() { return GetActionValue() / GetVisits(); }
double AZNode::GetPrior() { return prior_; }
unsigned AZNode::GetVisits() { return visits_; }

void AZNode::SetActionValue(double action_value) {
  action_value_ = action_value;
}
void AZNode::SetPrior(double prior) { prior_ = prior; }
void AZNode::SetVisits(unsigned visits) { visits_ = visits; }

}  // namespace alphazero
}  // namespace aithena
