/**
 * Copyright (C) 2020 All Right Reserved
 */

#pragma once

#include <torch/torch.h>
#include <memory>
#include <vector>

#include "chess/game.h"

namespace aithena {

class AZNode : public std::enable_shared_from_this<AZNode> {
 public:
  using AZNodePtr = std::shared_ptr<AZNode>;

  AZNode(chess::Game::GamePtr game, chess::State::StatePtr state,
         AZNodePtr parent = nullptr);

  chess::State::StatePtr GetState();
  AZNodePtr GetParent();
  std::vector<AZNodePtr> GetChildren();

  void Update(double value);

  void Expand(torch::Tensor priors);
  bool IsExpanded();

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
  AZNodePtr parent_;
  // The state encapsulated by this node.
  chess::State::StatePtr state_;
  // Whether the node has been expanded
  bool expanded_{false};

  // Edge statistics (in relation to the parent node)

  int visit_count_{0};
  double prior_{0};
  double action_value_{0};
};

}  // namespace aithena
