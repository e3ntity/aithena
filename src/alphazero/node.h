/*
Copyright 2020 All rights reserved.
*/

#include <memory>

#include "chess/game.h"
#include "chess/state.h"

namespace aithena {
namespace alphazero {

class AZNode;

typedef std::shared_ptr<AZNode> AZNodePtr;

class AZNode : public std::enable_shared_from_this<AZNode> {
 public:
  AZNode(::aithena::chess::State, AZNodePtr,
         std::shared_ptr<::aithena::chess::Game>);

  AZNode(const AZNode&) = delete;
  AZNode(AZNode&&) = delete;

  // Generates the child nodes of this node and makes them available.
  void Expand();

  // Returns whether the children of this node have been generated and are
  // available.
  bool IsExpanded();

  // Getters for node elements

  std::vector<AZNodePtr> GetChildren();
  AZNodePtr GetParent();
  ::aithena::chess::State GetState();

  // Getters and setters for node statistics

  double GetActionValue();
  double GetMeanActionValue();
  double GetPrior();
  unsigned GetVisits();
  void SetActionValue(double);
  void SetPrior(double);
  void SetVisits(unsigned);

 private:
  // The state of the game that this node captures.
  ::aithena::chess::State state_;
  // This node's parent.
  AZNodePtr parent_;
  // The game object containing chess rules.
  std::shared_ptr<::aithena::chess::Game> game_{};
  // Whether the children of this node have already been expanded.
  bool expanded_{false};
  // This node's child nodes.
  std::vector<AZNodePtr> children_{};

  // MCTS node statistics
  double action_value_{0};
  unsigned visits_{0};
  double prior_{0};
};

}  // namespace alphazero
}  // namespace aithena
