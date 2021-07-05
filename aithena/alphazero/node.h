/**
 * Copyright (C) 2020 All Right Reserved
 */

#ifndef AITHENA_ALPHAZERO_NODE_H_
#define AITHENA_ALPHAZERO_NODE_H_

#include <torch/torch.h>

#include <memory>
#include <vector>

#include "chess/game.h"

namespace aithena {

class AZNode : public std::enable_shared_from_this<AZNode> {
 public:
  using AZNodePtr = std::shared_ptr<AZNode>;

  AZNode(chess::Game::GamePtr game, chess::State::StatePtr state, AZNodePtr parent = nullptr);

  chess::State::StatePtr GetState();
  AZNodePtr GetParent();
  std::vector<AZNodePtr> GetChildren();
  void SetParent(AZNodePtr);

  int GetStateRepetitions();

  void SetPrior(double);
  // Updated the node's action value with the given value. If override is set to false, it is incorporated into the
  // current mean action value. If override is true, the current mean action value is set to the given value.
  void Update(double, bool override = false);

  void Expand();
  bool IsExpanded();

  bool IsTerminal();

  double GetMeanActionValue();
  double GetPrior();
  double GetTotalActionValue();
  int GetVisitCount();

 private:
  // The game rules for chess
  chess::Game::GamePtr game_;
  // The children nodes of this node.
  std::vector<AZNodePtr> children_{};
  // The parent node of this node.
  std::weak_ptr<AZNode> parent_;
  // The state encapsulated by this node.
  chess::State::StatePtr state_;
  // Whether the node has been expanded
  bool expanded_{false};

  // Edge statistics (in relation to the parent node)

  double action_value_{0};
  double prior_{0};
  int visit_count_{0};
};

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_NODE_H_
