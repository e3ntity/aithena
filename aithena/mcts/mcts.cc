/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <chrono>
#include <cmath>
#include <memory>

namespace aithena {

// Constructors

template <typename Game>
MCTS<Game>::MCTS(std::shared_ptr<Game> game) : game_{game} {}

template <typename Game>
void MCTS<Game>::Run(typename MCTSNode<Game>::NodePtr root, int simulations) {
  bm_.Start("Run");

  auto leaf = Select(root, UCTSelect);

  if (leaf->IsTerminal()) {
    Backpropagate(
        leaf, game_->GetStateResult(std::make_shared<typename Game::GameState>(
                  leaf->GetState())));

    bm_.End("Run");
    return;
  }

  auto child = RandomSelect(leaf);

  for (int simulation = 0; simulation < simulations; ++simulation) {
    int result = Simulate(child, RandomSelect);
    Backpropagate(child, result);
  }

  bm_.End("Run");
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::Select(
    typename MCTSNode<Game>::NodePtr start,
    typename MCTSNode<Game>::NodePtr (*next)(
        typename MCTSNode<Game>::NodePtr)) {
  bm_.Start("Select");

  typename MCTSNode<Game>::NodePtr current = start;

  while (!current->IsLeaf()) current = next(current);

  bm_.End("Select");

  return current;
}

template <typename Game>
int MCTS<Game>::Simulate(typename MCTSNode<Game>::NodePtr start,
                         typename MCTSNode<Game>::NodePtr (*next)(
                             typename MCTSNode<Game>::NodePtr)) {
  bm_.Start("Simulate");

  int i{0};
  auto current = start;
  while (!game_->IsTerminalState(
      std::make_shared<typename Game::GameState>(current->GetState()))) {
    if (!current->IsExpanded()) current->Expand();

    current = next(current);
    ++i;
  }

  int result = game_->GetStateResult(
      std::make_shared<typename Game::GameState>(current->GetState()));

  bm_.End("Simulate");

  return i % 2 == 0 ? result : -result;
}

template <typename Game>
void MCTS<Game>::Backpropagate(typename MCTSNode<Game>::NodePtr start,
                               int result) {
  bm_.Start("Backpropagate");

  auto current = start;
  int i{0};

  while (current != nullptr) {
    if (result == 0) {
      current->IncDraws();
    } else if ((result > 0 && i % 2 == 0) || (result < 0 && i % 2 == 1)) {
      current->IncLosses();
    } else {
      current->IncWins();
    }

    current = current->GetParent();
    ++i;
  }

  bm_.End("Backpropagate");
}

// Select strategies

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::RandomSelect(
    typename MCTSNode<Game>::NodePtr node) {
  if (!node->IsExpanded()) node->Expand();

  auto children = node->GetChildren();

  return children.at(rand() % children.size());
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::UCTSelect(
    typename MCTSNode<Game>::NodePtr node) {
  assert(!node->IsLeaf());
  assert(node->GetVisits() > 0);

  unsigned index{0};
  double maximum{0};
  double sum{0};

  auto children = node->GetChildren();
  std::vector<double> uct;

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    assert(child->GetVisits() > 0);

    double exploitation = double(child->GetWins()) / double(child->GetVisits());
    double exploration =
        sqrt(log(double(node->GetVisits())) / double(child->GetVisits()));
    double value = exploitation + M_SQRT2 * exploration;

    uct.push_back(value);
    sum += value;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  for (i = 0; i < children.size(); ++i)
    children.at(i)->SetUCTConfidence(uct.at(i) / sum);

  return children.at(index);
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::GreedySelect(
    typename MCTSNode<Game>::NodePtr node) {
  if (!node->IsExpanded()) return RandomSelect(node);

  int index{-1};
  double maximum{0};

  auto children = node->GetChildren();

  int i{0};
  for (; i < int(children.size()); ++i) {
    auto child = children.at(i);

    if (child->GetVisits() == 0) continue;

    double value = double(child->GetWins()) / double(child->GetVisits());

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  // We dont want to always return the first node if none has been visited.
  if (index < 0) return RandomSelect(node);

  return children.at(index);
}

}  // namespace aithena
