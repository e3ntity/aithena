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
class MCTSNode : public std::enable_shared_from_this<MCTSNode<Game>> {
 public:
  using NodePtr = std::shared_ptr<MCTSNode<Game>>;
  using NodePtrList = std::vector<NodePtr>;

  // Initialize with the default game state.
  explicit MCTSNode(std::shared_ptr<Game>);
  // Initialize with a specific state.
  MCTSNode(std::shared_ptr<Game>, typename Game::GameState, NodePtr parent);

  // Operators
  MCTSNode<Game>& operator=(const MCTSNode& node);

  // Two nodes are considered the same if they represent the same state.
  bool operator==(const MCTSNode& node);
  bool operator!=(const MCTSNode& node);

  // Getter
  typename Game::GameState& GetState();
  NodePtrList GetChildren();
  NodePtr GetParent();

  // Generates child nodes.
  void Expand();
  // Returns whether the node has all child nodes generated.
  // children_ may have members without the node being expanded if e.g. a
  // garbage collector is used to remove untouched nodes.
  bool IsExpanded();
  // Returns true if any of the node's children have not been visited or if the
  // node is terminal.
  bool IsLeaf();
  // Returns whether this node is a terminal node.
  bool IsTerminal();

  // Increments the win count and visit count by one.
  void IncWins();
  // Increments the draw count and visit count by one.
  void IncDraws();
  // Increments the loss count and visit count by one.
  void IncLosses();

  unsigned GetWins();
  unsigned GetDraws();
  unsigned GetLosses();
  unsigned GetVisits();

 private:
  // This node's child nodes.
  NodePtrList children_;
  // This node's parent.
  NodePtr parent_;
  // An object capturing the rules of the game.
  std::shared_ptr<Game> game_;
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
