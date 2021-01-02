/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <cmath>
#include <iostream>

namespace aithena {

// Constructors

template <typename Game>
MCTS<Game>::MCTS(Game game) : game_{game} {}

template <typename Game>
void MCTS<Game>::Run(
  typename MCTSNode<Game>::NodePtr root,
  unsigned rounds,
  unsigned simulations
) {
  for (unsigned round = 0; round < rounds; ++round) {
    auto leaf = Select(root, UCTSelect);

    if (leaf->IsTerminal()) continue;

    if (!leaf->IsExpanded()) leaf->Expand();

    auto child = RandomSelect(leaf);

    for (unsigned simulation = 0; simulation < simulations; ++simulation) {
      int result = Simulate(child, RandomSelect);
      Backpropagate(child, result);

      std::cout << "." << std::flush;
    }
    std::cout << std::endl;
  }
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::Select(
  typename MCTSNode<Game>::NodePtr start,
  typename MCTSNode<Game>::NodePtr (*next)(typename MCTSNode<Game>::NodePtr)
) {
  typename MCTSNode<Game>::NodePtr current = start;

  while (!current->IsLeaf())
    current = next(current);

  return current;
}

template <typename Game>
int MCTS<Game>::Simulate(
  typename MCTSNode<Game>::NodePtr start,
  typename MCTSNode<Game>::NodePtr (*next)(typename MCTSNode<Game>::NodePtr)
) {
  auto current = start;

  while (!game_.IsTerminalState(current->GetState())) {
    if (!current->IsExpanded()) current->Expand();

    current = next(current);
  }

  return game_.GetStateResult(current->GetState());
}

template <typename Game>
void MCTS<Game>::Backpropagate(typename MCTSNode<Game>::NodePtr start, int result) {
  auto current = start;

  for (int i = 0; current->GetParent() != nullptr; ++i) {
    if (result == 0)
      current->IncDraws();
    else if ((result > 0 && i % 2 == 0) || (result < 0 && i % 2 == 1))
      current->IncWins();
    else
      current->IncLosses();

    current = current->GetParent();
  }
}

// Select strategies

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::RandomSelect(
  typename MCTSNode<Game>::NodePtr node
) {
  assert(node->IsExpanded());

  auto children = node->GetChildren();

  return children.at(rand() % children.size());
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::UCTSelect(
  typename MCTSNode<Game>::NodePtr node
) {
  assert(node->IsExpanded());

  unsigned index{0};
  unsigned maximum{0};

  auto children = node->GetChildren();

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    assert(child->GetVisits() > 0);

    unsigned exploitation = child->GetWins() / child->GetVisits();
    unsigned exploration = sqrt(log(node->GetVisits()) / child->GetVisits());
    unsigned value = exploitation + exploration;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::GreedySelect(typename MCTSNode<Game>::NodePtr node) {
  if (!node->IsExpanded()) return RandomSelect(node);

  unsigned index{0};
  unsigned maximum{0};

  auto children = node->GetChildren();

  for (unsigned i = 0; i < children.size(); ++i) {
    auto child = children.at(i);

    if (child->GetVisits() == 0) continue;

    unsigned value = child->GetWins() / child->GetVisits();

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  // We dont want to always return the first node if none has been visited.
  if (index == 0 && (children.at(index))->GetVisits() == 0)
    return RandomSelect(node);

  return children.at(index);
}

}  // namespace aithena
