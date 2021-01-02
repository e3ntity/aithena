/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_MCTS_MCTS_H_
#define SRC_MCTS_MCTS_H_

#include "mcts/node.h"

namespace aithena {

// Monte Carlo Tree Search
template <typename Game>
class MCTS {
 public:
  using Node = MCTSNode<Game>;

  MCTS(Game);

  // Runs the MCTS algorithms for n rounds, using m simulations each round,
  // starting at the given node.
  void Run(
    typename Node::NodePtr,
    unsigned rounds = 1,
    unsigned simulations = 1
  );

  // MCTS Selection, starting at some node and selecting successive children
  // according to the search strategy until a leaf node / terminal node is
  // reached.
  typename Node::NodePtr Select(
    typename Node::NodePtr,
    typename Node::NodePtr (*next)(typename Node::NodePtr)
  );

  // Simulates a playout using some selection strategy, starting from some node.
  // Returns the result of the playout from the perspective of the node from
  // which it was started.
  int Simulate(
    typename Node::NodePtr,
    typename Node::NodePtr (*next)(typename Node::NodePtr)
  );

  // Backpropagates the result of a playout from the node where it was started
  // to the root.
  static void Backpropagate(typename Node::NodePtr, int);

  // Strategies for selecting the next node.

  // Selects a random next node. Make sure to previously seed the prng with
  // srand(). Assumes that the node has already been expanded.
  static typename Node::NodePtr RandomSelect(typename Node::NodePtr);

  // Selects the next node according to the upper confidence bound.
  // Assumes that the node has already been expanded and that each child node
  // has been visited at least once.
  static typename Node::NodePtr UCTSelect(typename Node::NodePtr);

  // Selects the next node greedily.
  static typename Node::NodePtr GreedySelect(typename Node::NodePtr);
 private:
  Game game_;
};

template class MCTS<::aithena::chess::Game>;

}  // namespace aithena

#endif  // SRC_MCTS_MCTS_H_
