/*
Copyright 2020 All rights reserved.
*/

#include "mcts/mcts.h"

#include <chrono>
#include <cmath>
#include <iostream>
std::string PrintMarkedBoard1(aithena::chess::State state,
                             aithena::BoardPlane marker) {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;

  std::string turn = state.GetPlayer() == aithena::chess::Player::kWhite
                     ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1)
                            ? "\033[1;47m"
                            : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white = piece.player ==
              static_cast<unsigned>(aithena::chess::Player::kWhite);
      std::string s_piece;

      // Figure icon
      switch (piece.figure) {
      case static_cast<unsigned>(aithena::chess::Figure::kKing):
        s_piece = white ? "♔" : "♚";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kQueen):
        s_piece = white ? "♕" : "♛";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kRook):
        s_piece = white ? "♖" : "♜";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kBishop):
        s_piece = white ? "♗" : "♝";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kKnight):
        s_piece = white ? "♘" : "♞";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kPawn):
        s_piece = white ? "♙" : "♟︎";
        break;
      default:
        s_piece += std::to_string(piece.figure);
        break;
      }

      bool marked = marker.get(x, y);

      if (piece == aithena::kEmptyPiece) {
        repr << s_color << (marked ? "\033[31m- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? "\033[31m-" : " ") << s_color << s_piece
             << "\033[24m" << (marked ? "\033[31m-" : " ") << "\033[0m";
      }
    }
  }
  repr << std::endl << "   A  B  C  D  E  F  G  H" << std::endl;

  return repr.str();
}

std::string PrintBoard1(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard1(state, marker);
}
namespace aithena {

// Constructors

template <typename Game>
MCTS<Game>::MCTS(Game game) : game_{game} {}

template <typename Game>
void MCTS<Game>::Run(
  typename MCTSNode<Game>::NodePtr root,
  unsigned rounds,
  unsigned simulations
) {
  for (unsigned round = 0; round < rounds; ++round) {
    auto leaf = Select(root, UCTSelect);

    if (leaf->IsTerminal()) continue;

    if (!leaf->IsExpanded()) leaf->Expand();

    auto child = RandomSelect(leaf);

    for (unsigned simulation = 0; simulation < simulations; ++simulation) {
      int result = Simulate(child, RandomSelect);
      Backpropagate(child, result);

      std::cout << "." << std::flush;
    }
    std::cout << std::endl;
  }
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
  auto current = start;

  auto bm_start = std::chrono::high_resolution_clock::now();

  while (!game_.IsTerminalState(current->GetState())) {
    //std::cout << PrintBoard1(current->GetState()) << "\n";
    if (!current->IsExpanded()) current->Expand();

    current = next(current);
  }

  auto bm_end = std::chrono::high_resolution_clock::now();
  time_simulate.push_back(
    std::chrono::duration_cast<std::chrono::milliseconds>(bm_end - bm_start)
  );

  return game_.GetStateResult(current->GetState());
}

template <typename Game>
void MCTS<Game>::Backpropagate(typename MCTSNode<Game>::NodePtr start, int result) {
  auto current = start;

  auto bm_start = std::chrono::high_resolution_clock::now();

  for (int i = 0; current != nullptr; ++i) {
    if (result == 0)
      current->IncDraws();
    else if (result > 0)
      current->IncWins();
    else
      current->IncLosses();

    current = current->GetParent();
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
  assert(node->IsExpanded());

  auto children = node->GetChildren();

  return children.at(rand() % children.size());
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::UCTSelect(
  typename MCTSNode<Game>::NodePtr node
) {
  assert(node->IsExpanded());

  unsigned index{0};
  unsigned maximum{0};

  auto children = node->GetChildren();

  unsigned i{0};
  for (; i < children.size(); ++i) {
    auto child = children.at(i);

    assert(child->GetVisits() > 0);

    unsigned exploitation = child->GetWins() / child->GetVisits();
    unsigned exploration = sqrt(log(node->GetVisits()) / child->GetVisits());
    unsigned value = exploitation + exploration;

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  return children.at(index);
}

template <typename Game>
typename MCTSNode<Game>::NodePtr MCTS<Game>::GreedySelect(typename MCTSNode<Game>::NodePtr node) {
  if (!node->IsExpanded()) return RandomSelect(node);

  unsigned index{0};
  double maximum{0};

  auto children = node->GetChildren();

  for (unsigned i = 0; i < children.size(); ++i) {
    auto child = children.at(i);

    if (child->GetVisits() == 0) continue;

    double value = static_cast<double>(child->GetWins()) / child->GetVisits();

    if (value <= maximum) continue;

    index = i;
    maximum = value;
  }

  // We dont want to always return the first node if none has been visited.
  if (index == 0 && (children.at(index))->GetVisits() == 0)
    return RandomSelect(node);

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
