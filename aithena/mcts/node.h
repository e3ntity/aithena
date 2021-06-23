/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_MCTS_NODE_H_
#define AITHENA_MCTS_NODE_H_

#include <memory>
#include <vector>

#include "chess/game.h"
#include "game/game.h"

namespace aithena {

class MCTSNode : public std::enable_shared_from_this<MCTSNode> {
 public:
  using MCTSNodePtr = std::shared_ptr<MCTSNode>;

  MCTSNode(chess::Game::GamePtr game, chess::State::StatePtr state, MCTSNodePtr parent = nullptr);

  chess::State::StatePtr GetState();
  MCTSNodePtr GetParent();
  std::vector<MCTSNodePtr> GetChildren();
  void SetParent(MCTSNodePtr);

  void Update(int);
  void Expand();
  bool IsExpanded();
  bool IsLeaf();
  bool IsTerminal();

  double GetMeanWinCount();
  int GetTotalWinCount();
  int GetVisitCount();

 private:
  // The game rules for chess
  chess::Game::GamePtr game_;
  // The children nodes of this node.
  std::vector<MCTSNodePtr> children_{};
  // The parent node of this node.
  std::weak_ptr<MCTSNode> parent_;
  // The state encapsulated by this node.
  chess::State::StatePtr state_;
  // Whether the node has been expanded
  bool expanded_{false};

  int win_count_{0};
  int visit_count_{0};
};

}  // namespace aithena

#endif  // AITHENA_MCTS_NODE_H_
