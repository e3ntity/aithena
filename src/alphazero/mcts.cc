/*
Copyright 2020 All rights reserved.
*/

#include "alphazero/mcts.h"

#include <cmath>

namespace aithena {
namespace alphazero {

AZMCTS::AZMCTS(AZNodePtr root) : root_{root} {}

void AZMCTS::Simulate() {
  auto current_node = root_;

  while (current_node->IsExpanded()) current_node = PUCTSelect(current_node);

  current_node->Expand();

  // TODO: update nodes and feed leaf's state into neural network
  // priors, value = nn.feed_forward(current_node.GetState())
  // for child in current_node->GetChildren():
  //   prior = None
  //   for p in priors:
  //     if p.state != child->GetState():
  //       continue
  //     prior = p
  //     break
  //   if not prior: // invalid action selected, ignore / penalize?
  //   child.SetPrior(priors)
  // nn_queue.add(current_node->GetState(), value)
}

// Exploration rates used for PUCT.
// TODO: make configurable.
double c_base = 0.05;
double c_init = 0.1;

AZNodePtr AZMCTS::PUCTSelect(AZNodePtr node) {
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
