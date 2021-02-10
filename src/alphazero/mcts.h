/*
Copyright 2020 All rights reserved.
*/

#include "alphazero/neural_network.h"
#include "alphazero/node.h"

namespace aithena {
namespace alphazero {

// Wraps an alpha zero MCTS node to learn over the tree generated from the node.
class AZMCTS {
 public:
  // Constructs an alpha zero MCTS object given an alpha zero root node.
  AZMCTS(AZNodePtr);

  void Simulate();

 private:
  // The root node wrapped by the MCTS object.
  AZNodePtr root_;

  // Selects the next node according to the PUTCT algorithm.
  // Assumes that the node has already been expanded and that each child node
  // has been visited at least once.
  AZNodePtr PUCTSelect(AZNodePtr node);
};

}  // namespace alphazero
}
