/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>

namespace aithena {

// Constructors

template <typename Game>
MCTS<Game>::MCTS(std::shared_ptr<Game> game) : game_{game} {}

template <typename Game>
void MCTS<Game>::Run(
  typename MCTSNode<Game>::NodePtr root,
  int rounds,
  int simulations
) {
  for (int round = 0; round < rounds; ++round) {
    double percentage = 100.0 * double(round) / double(rounds);
    std::cout << "Running MCTS: " << std::setprecision(3) << percentage
              << "%   \r" << std::flush;

    //for (auto child : root->GetChildren()) {
    //  std::cout << std::setfill(' ') << std::setw(4) << child->GetVisits();
    //  std::cout << "(" << std::setfill(' ') << std::setw(2) << child->GetWins() / child->GetVisits() << ")";
    //}
    //std::cout << std::endl;

    auto leaf = Select(root, UCTSelect);

    if (leaf->IsTerminal()) {
      Backpropagate(leaf, game_->GetStateResult(leaf->GetState()));
      continue;
    }

    auto child = RandomSelect(leaf);

    for (int simulation = 0; simulation < simulations; ++simulation) {
      int result = Simulate(child, RandomSelect);
      Backpropagate(child, result);
    }
  }
  std::cout << std::endl;
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::Select(
  typename MCTSNode<Game>::NodePtr start,
  typename MCTSNode<Game>::NodePtr (*next)(typename MCTSNode<Game>::NodePtr)
) {
  typename MCTSNode<Game>::NodePtr current = start;

  auto bm_start = std::chrono::high_resolution_clock::now();

  while (!current->IsLeaf())
    current = next(current);

  auto bm_end = std::chrono::high_resolution_clock::now();
  time_select.push_back(
    std::chrono::duration_cast<std::chrono::milliseconds>(bm_end - bm_start)
  );

  return current;
}

template <typename Game>
int MCTS<Game>::Simulate(
  typename MCTSNode<Game>::NodePtr start,
  typename MCTSNode<Game>::NodePtr (*next)(typename MCTSNode<Game>::NodePtr)
) {
  auto bm_start = std::chrono::high_resolution_clock::now();

  int i{0};
  auto current = start;
  while (!game_->IsTerminalState(current->GetState())) {
    if (!current->IsExpanded()) current->Expand();

    current = next(current);
    ++i;
  }

  int result = game_->GetStateResult(current->GetState());

  auto bm_end = std::chrono::high_resolution_clock::now();
  time_simulate.push_back(
    std::chrono::duration_cast<std::chrono::milliseconds>(bm_end - bm_start)
  );

  return i % 2 == 0 ? result : -result;
}

template <typename Game>
void MCTS<Game>::Backpropagate(typename MCTSNode<Game>::NodePtr start, int result) {
  auto bm_start = std::chrono::high_resolution_clock::now();

  auto current = start;
  int i{0};

  while (current != nullptr) {
    if (result == 0) {
      current->IncDraws();
    } else if ((result > 0 && i % 2 == 0) || (result < 0 && i % 2 == 1)) {
      current->IncLosses();
    } else {
      current->IncWins();
    }

    current = current->GetParent();
    ++i;
  }

  auto bm_end = std::chrono::high_resolution_clock::now();
  time_backpropagate.push_back(
    std::chrono::duration_cast<std::chrono::milliseconds>(bm_end - bm_start)
  );
}

// Select strategies

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::RandomSelect(
  typename MCTSNode<Game>::NodePtr node
) {
  if (!node->IsExpanded()) node->Expand();

  auto children = node->GetChildren();

  return children.at(rand() % children.size());
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::UCTSelect(
  typename MCTSNode<Game>::NodePtr node
) {
  assert(!node->IsLeaf());
  assert(node->GetVisits() > 0);

  unsigned index{0};
  double maximum{0};

  auto children = node->GetChildren();

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    assert(child->GetVisits() > 0);

    double exploitation = child->GetWins() / child->GetVisits();
    double exploration = sqrt(log(node->GetVisits()) / child->GetVisits());
    double value = exploitation + exploration;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::GreedySelect(
  typename MCTSNode<Game>::NodePtr node
) {
  if (!node->IsExpanded()) return RandomSelect(node);

  int index{ -1};
  double maximum{0};

  auto children = node->GetChildren();

  int i{0};
  for (; i < int(children.size()); ++i) {
    auto child = children.at(i);

    if (child->GetVisits() == 0) continue;

    double value = child->GetWins() / child->GetVisits();

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  // We dont want to always return the first node if none has been visited.
  if (index < 0) return RandomSelect(node);

  return children.at(index);
}

template <typename Game>
double MCTS<Game>::BenchmarkSelect() {
  std::chrono::milliseconds sum{0};

  for (auto point : time_select) sum += point;

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(sum / time_select.size());

  return static_cast<double>(duration.count()) / 1000.;
}

template <typename Game>
double MCTS<Game>::BenchmarkSimulate() {
  std::chrono::milliseconds sum{0};

  for (auto point : time_simulate) sum += point;

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(sum / time_simulate.size());

  return static_cast<double>(duration.count()) / 1000.;
}

template <typename Game>
double MCTS<Game>::BenchmarkBackpropagate() {
  std::chrono::milliseconds sum{0};

  for (auto point : time_backpropagate) sum += point;

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(sum / time_backpropagate.size());

  return static_cast<double>(duration.count()) / 1000.;
}

}  // namespace aithena
