/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_MCTS_NODE_H_
#define SRC_MCTS_NODE_H_

#include <memory>
#include <vector>

#include "game/game.h"
#include "chess/game.h"

namespace aithena {

template <typename Game>
class MCTSNode {
 public:
  using Child = std::shared_ptr<MCTSNode<Game>>;
  using ChildList = std::vector<Child>;

  // Initialize with the default game state.
  explicit MCTSNode(Game&);
  // Initialize with a specific state.
  MCTSNode(Game&, typename Game::GameState);

  // Operators
  MCTSNode<Game>& operator=(const MCTSNode& node);

  // Getter
  typename Game::GameState GetState();
  ChildList GetChildren();

  // Generates child nodes.
  void Expand();
  // Returns whether the node has all child nodes generated.
  // children_ may have members without the node being expanded if e.g. a
  // garbage collector is used to remove untouched nodes.
  bool IsExpanded();
  // Returns whether this node is a terminal node.
  bool IsTerminal();

  unsigned GetWins();
  unsigned GetDraws();
  unsigned GetVisits();

 private:
  // This node's child nodes.
  ChildList children_;
  // An object capturing the rules of the game.
  Game& game_;
  // The state of the game that this node captures.
  typename Game::GameState state_;
  // Whether this node has all possible child nodes in children_.
  bool expanded_{false};

  unsigned wins_{0};
  unsigned draws_{0};
  unsigned visits_{0};
};

template class MCTSNode<::aithena::chess::Game>;

}  // namespace aithena

#endif  // SRC_MCTS_NODE_H_
