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
  using Node = aithena::MCTSNode<aithena::chess::Game>;

  MCTS(Game, Node&);

  Node& GetRoot();

  // Strategies for selecting the next node.

  // Selects a random next node. Make sure to previously seed the prng with
  // srand()
  static Node::Child RandomSelect(Node, Node::ChildList);

  // Selects the next node according to the upper confidence bound.
  // Assumes that the child has already been visited at least once.
  static Node::Child UCTSelect(Node, Node::ChildList);
 private:
  Game game_;
  Node root_;
};

template class MCTS<::aithena::chess::Game>;

}  // namespace aithena

#endif  // SRC_MCTS_MCTS_H_
