/*
Copyright 2020 All rights reserved.
*/

#include "mcts/node.h";

namespace aithena {
namespace alphazero {

template <typename Game>
class AZMCTSNode : public MCTSNode<Game> {
 public:
  void IncWins() = delete;
  void IncDraws() = delete;
  void IncLosses() = delete;

 private:
  // Default MCTS node statistics are not used.
  double action_value_{0};
  unsigned visits_{0};
  double mean_action_value_{0};
  double prior_{0};
}

}  // namespace alphazero
}
