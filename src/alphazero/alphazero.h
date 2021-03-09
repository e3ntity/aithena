#ifndef AITHENA_ALPHAZERO_ALPHAZERO
#define AITHENA_ALPHAZERO_ALPHAZERO

#include "mcts/mcts.h"

namespace aithena {
namespace alphazero {

class AlphaZero : ::aithena::MCTS<::aithena::chess::Game> {
 public:
  void Run(typename Node::NodePtr, int simulations) = delete;
  void Run(typename Node::NodePtr);

  void Backpropagate(typename Node::NodePtr, int);
  static typename Node::NodePtr PUCTSelect(typename Node::NodePtr);

 private:
};

}  // alphazero
}  // aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO
