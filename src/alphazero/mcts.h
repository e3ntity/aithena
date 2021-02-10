/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

namespace aithena {
namespace alphazero {

template <typename Game>
class AZMCTS : public MCTSNode<Game> {
 public:
  // TODO: implement
  typename Node::NodePtr Select(
      typename Node::NodePtr,
      typename Node::NodePtr (*next)(typename Node::NodePtr)) override;

  // TODO: implement
  int Simulate(typename Node::NodePtr,
               typename Node::NodePtr (*next)(typename Node::NodePtr)) override;

  // TODO: implement
  void Backpropagate(typename Node::NodePtr, int) override;

  // Selects the next node according to the PUTCT algorithm.
  // Assumes that the node has already been expanded and that each child node
  // has been visited at least once.
  static typename Node::NodePtr PUCTSelect(typename Node::NodePtr);

 private:
}

}  // namespace alphazero
}
