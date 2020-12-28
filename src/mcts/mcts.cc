/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <cmath>

namespace aithena {

// Constructors

template <typename Game>
MCTS<Game>::MCTS(Game game, Node& root) : game_{game}, root_{root} {}

template <typename Game>
aithena::MCTSNode<aithena::chess::Game>& MCTS<Game>::GetRoot() {
  return root_;
}

// Select strategies

template <typename Game>
MCTS<Game>::Node::Child MCTS<Game>::RandomSelect(MCTS<Game>::Node node,
    MCTS<Game>::Node::ChildList children) {
  return children.at(rand_r() % children.size());
}

template <typename Game>
MCTS<Game>::Node::Child MCTS<Game>::UCTSelect(MCTS<Game>::Node node,
    MCTS<Game>::Node::ChildList children) {
  unsigned index{0};
  unsigned maximum{0};

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = *children.at(i);

    assert(child.GetVisits() > 0);

    unsigned exploitation = child.GetWins() / child.GetVisits();
    unsigned exploration = sqrt(log(node.GetVisits()) / child.GetVisits());
    unsigned value = exploitation + exploration;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

}  // namespace aithena
