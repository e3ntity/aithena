/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_ALPHAZERO_NODE
#define AITHENA_ALPHAZERO_NODE

#include <torch/torch.h>
#include <memory>

#include "alphazero/neural_network.h"
#include "chess/game.h"
#include "mcts/node.h"

namespace aithena {
namespace alphazero {

class AZNode : public std::enable_shared_from_this<AZNode> {
 public:
  using NodePtr = std::shared_ptr<AZNode>;
  using NodePtrList = std::vector<NodePtr>;

  AZNode(std::shared_ptr<chess::Game>, AZNeuralNetwork nn);
  AZNode(std::shared_ptr<chess::Game>, chess::Game::GameState, NodePtr,
         AZNeuralNetwork nn);

  AZNode& operator=(const AZNode& node) = delete;
  bool operator==(const AZNode& node) = delete;
  bool operator!=(const AZNode& node) = delete;

  torch::Tensor GetNNInput();

  // Tree node functions
  void Expand();

  bool IsExpanded();
  bool IsLeaf();
  bool IsTerminal();

  chess::Game::GameState& GetState();
  NodePtrList GetChildren();
  NodePtr GetParent();

  // Setters and getters for node statistics
  void SetPrior(double);
  void SetValue(double);
  void IncVisits();
  double GetPrior();
  double GetValue();
  int GetVisits();

 private:
  // This node's child nodes.
  NodePtrList children_;
  // This node's parent.
  NodePtr parent_;
  // An object capturing the rules of the game.
  std::shared_ptr<chess::Game> game_;
  // The state of the game that this node captures.
  typename chess::Game::GameState state_;
  // Whether this node has all possible child nodes in children_.
  bool expanded_{false};

  AZNeuralNetwork nn_;

  // Statistics for the az node.
  int visits_{0};
  double prior_{0};
  double value_{0};
};

}  // aithena
}  // alphazero

#endif  // AITHENA_ALPHAZERO_NODE
