/*
Copyright 2020 All rights reserved.
*/

#include "mcts/node.h"

#include <memory>

namespace aithena {

// Constructors

template <typename Game>
MCTSNode<Game>::MCTSNode(std::shared_ptr<Game> game)
  : children_{},
    parent_{nullptr},
    game_{game},
    state_{game->GetInitialState()} {}

template <typename Game>
MCTSNode<Game>::MCTSNode(std::shared_ptr<Game> game,
                         typename Game::GameState state, NodePtr parent)
  : children_{}, parent_{parent}, game_{game}, state_{state} {}

// Operators

template <typename Game>
MCTSNode<Game>& MCTSNode<Game>::operator=(const MCTSNode& node) {
  children_ = node.children_;
  game_ = node.game_;
  state_ = node.state_;
  expanded_ = node.expanded_;

  return *this;
}

template <typename Game>
bool MCTSNode<Game>::operator==(const MCTSNode<Game>& other) {
  return state_ == other.state_;
}

template <typename Game>
bool MCTSNode<Game>::operator!=(const MCTSNode<Game>& other) {
  return !operator==(other);
}

// Getters

template <typename Game>
typename Game::GameState& MCTSNode<Game>::GetState() {
  return state_;
}

template <typename Game>
std::vector<std::shared_ptr<MCTSNode<Game>>> MCTSNode<Game>::GetChildren() {
  return children_;
}

template <typename Game>
std::shared_ptr<MCTSNode<Game>> MCTSNode<Game>::GetParent() {
  return parent_;
}

// Tree operations

template <typename Game>
void MCTSNode<Game>::Expand() {
  auto moves = game_->GetLegalActions(state_);

  for (auto state : moves) {
    children_.push_back(
      std::shared_ptr<MCTSNode<Game>>(
        new MCTSNode<Game>(game_, state, this->shared_from_this())
      )
    );
  }

  for (auto child : children_)
    child->SetUCTConfidence(1 / children_.size());

  expanded_ = true;
}

template <typename Game>
bool MCTSNode<Game>::IsExpanded() {
  return expanded_;
}

template <typename Game>
bool MCTSNode<Game>::IsLeaf() {
  if (!IsExpanded() || IsTerminal()) return true;

  for (auto child : GetChildren()) {
    if (child->GetVisits() == 0) return true;
  }

  return false;
}

template <typename Game>
bool MCTSNode<Game>::IsTerminal() {
  //if (IsExpanded()) return children_.size() == 0;  // Cheap check

  return game_->IsTerminalState(state_);
}

template <typename Game>
void MCTSNode<Game>::IncWins() {
  wins_++;
  visits_++;
}

template <typename Game>
void MCTSNode<Game>::IncDraws() {
  draws_++;
  visits_++;
}

template <typename Game>
void MCTSNode<Game>::IncLosses() {
  visits_++;
}

template <typename Game>
unsigned MCTSNode<Game>::GetWins() {
  return wins_;
}

template <typename Game>
unsigned MCTSNode<Game>::GetDraws() {
  return draws_;
}

template <typename Game>
unsigned MCTSNode<Game>::GetLosses() {
  return GetVisits() - GetDraws() - GetWins();
}

template <typename Game>
unsigned MCTSNode<Game>::GetVisits() {
  return visits_;
}

template <typename Game>
void MCTSNode<Game>::SetUCTConfidence(double confidence) {
  UCTConfidence_ = confidence;
}

template <typename Game>
double MCTSNode<Game>::GetUCTConfidence() { return UCTConfidence_; }

}  // namespace aithena
