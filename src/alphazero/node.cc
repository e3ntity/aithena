#include "alphazero/node.h"
#include "alphazero/move.h"

namespace aithena {
namespace alphazero {

AZNode::AZNode(std::shared_ptr<chess::Game> game, AZNeuralNetwork nn)
    : children_{},
      parent_{nullptr},
      game_{game},
      state_{game->GetInitialState()},
      nn_{nn} {};

AZNode::AZNode(std::shared_ptr<chess::Game> game, chess::Game::GameState state,
               NodePtr parent, AZNeuralNetwork nn)
    : children_{}, parent_{parent}, game_{game}, state_{state}, nn_{nn} {};

torch::Tensor AZNode::GetNNInput() {
  // TODO: implement
  return torch::Tensor();
};

void AZNode::Expand() {
  auto moves = game_->GetLegalActions(state_);

  torch::Tensor output = nn_->forward(GetNNInput());
  double value = output[0][-1].item<double>();

  for (auto state : moves) {
    double prior = output[0][ChessMoveValue(state_, state)].item<double>();

    NodePtr child = std::shared_ptr<AZNode>(
        new AZNode(game_, state, this->shared_from_this(), nn_));

    child->SetPrior(prior);
    child->SetValue(value);

    children_.push_back(child);
  }

  expanded_ = true;
}

bool AZNode::IsExpanded() { return expanded_; }
bool AZNode::IsTerminal() { return game_->IsTerminalState(state_); }
bool AZNode::IsLeaf() {
  if (!IsExpanded() || IsTerminal()) return true;

  for (auto child : GetChildren()) {
    if (child->GetVisits() == 0) return true;
  }

  return false;
}
chess::Game::GameState& AZNode::GetState() { return state_; }
AZNode::NodePtrList AZNode::GetChildren() { return children_; }
AZNode::NodePtr AZNode::GetParent() { return parent_; }

void AZNode::SetPrior(double prior) { prior_ = prior; }
void AZNode::SetValue(double value) { value_ = value; }
void AZNode::IncVisits() { ++visits_; }
double AZNode::GetPrior() { return prior_; }
double AZNode::GetValue() { return value_; }
int AZNode::GetVisits() { return visits_; }

}  // alphazero
}  // aithena