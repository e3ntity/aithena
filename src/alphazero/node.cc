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

torch::Tensor AZNode::GetNNInput(int history_length) {
  torch::Tensor output = state_.PlanesAsTensor();

  if (history_length <= 1) return output;

  if (history_length == 8)
    output = torch::cat({state_.DetailsAsTensor(), output}, 1);

  if (parent_ != nullptr)
    return torch::cat({output, parent_->GetNNInput(history_length - 1)}, 1);

  torch::Tensor empty_tensor =
      torch::zeros({1, state_.GetBoard().GetFigureCount() * 2 + 2,
                    static_cast<long>(state_.GetBoard().GetWidth()),
                    static_cast<long>(state_.GetBoard().GetHeight())});

  for (int i = 0; i < history_length - 1; ++i)
    output = torch::cat({output, empty_tensor}, 1);

  return output;
};

torch::Tensor AZNode::GetChildrenPriors() {
  torch::Tensor priors =
      torch::zeros({1, MaxChessMoveValue(state_.GetBoard().GetWidth(),
                                         state_.GetBoard().GetHeight())});

  for (auto child : children_) {
    priors.index_put_({0, ChessMoveValue(state_, child->GetState())},
                      double(child->GetVisits()) / double(GetVisits()));
  }

  return priors;
}

double AZNode::Expand() {
  auto moves = game_->GetLegalActions(state_);

  torch::Tensor output = nn_->forward(GetNNInput());
  double value = output[0][-1].item<double>();

  for (auto state : moves) {
    double prior = output[0][ChessMoveValue(state_, state)].item<double>();

    NodePtr child = std::shared_ptr<AZNode>(
        new AZNode(game_, state, this->shared_from_this(), nn_));

    child->SetPrior(prior);

    children_.push_back(child);
  }

  expanded_ = true;

  return value;
}

bool AZNode::IsExpanded() { return expanded_; }
bool AZNode::IsTerminal() { return game_->IsTerminalState(state_); }

bool AZNode::IsLeaf() {
  if (!IsExpanded() || IsTerminal()) return true;

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
double AZNode::GetMeanValue() { return value_ / visits_; }
int AZNode::GetVisits() { return visits_; }

}  // alphazero
}  // aithena