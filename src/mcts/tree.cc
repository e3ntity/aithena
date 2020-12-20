#include "mcts/tree.h"

#include <memory>

namespace aithena {

// Constructors

template <typename Game>
MCTSNode<Game>::MCTSNode(Game& game)
  : children_{}, game_{game}, state_{game.GetInitialState()} {}

template <typename Game>
MCTSNode<Game>::MCTSNode(Game& game, typename Game::GameState state)
  : children_{}, game_{game}, state_{state} {}

// Operators

template <typename Game>
MCTSNode<Game>& MCTSNode<Game>::operator=(const MCTSNode& node) {
  children_ = node.children_;
  game_ = node.game_;
  state_ = node.state_;
  expanded_ = node.expanded_;

  return *this;
}

// Getters

template <typename Game>
typename Game::GameState MCTSNode<Game>::GetState() {
  return state_;
}

template <typename Game>
std::vector<std::shared_ptr<MCTSNode<Game>>> MCTSNode<Game>::GetChildren() {
  return children_;
}

// Tree operations

template <typename Game>
void MCTSNode<Game>::Expand() {
  auto moves = game_.GetLegalActions(state_);

  for (auto state : moves) {
    children_.push_back(
      std::shared_ptr<MCTSNode<Game>>(new MCTSNode<Game>(game_, state)));
  }

  expanded_ = true;
}

template <typename Game>
bool MCTSNode<Game>::IsExpanded() {
  return expanded_;
}

template <typename Game>
bool MCTSNode<Game>::IsTerminal() {
  if (IsExpanded()) return children_.size() == 0; // Cheap check

  return game_.IsTerminalState(state_);
}

}
