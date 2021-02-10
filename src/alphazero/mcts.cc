/*
Copyright 2020 All rights reserved.
*/

#include "alphazero/mcts.h"

#include <cmath>

namespace aithena {
namespace alphazero {

// Exploration rates used for PUCT.
// TODO: make configurable.
double c_base = 0.05;
double c_init = 0.1;

template <typename Game>
typename AZMCTSNode<Game>::NodePtr AZMCTS<Game>::PUCTSelect(
    typename AZMCTSNode<Game>::NodePtr node) {
  assert(node->IsExpanded());

  unsigned index{0};
  double maximum{0};

  auto children = node->GetChildren();

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    assert(child->GetVisits() > 0);

    double exploration =
        log((1 + child->GetVisits() + c_base) / c_base) + c_init;
    double value = exploration * child->GetPrior() * sqrt(child->GetVisits()) /
                   (1 + child->GetVisits());

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

}  // namespace alphazero
}  // namespace aithena
