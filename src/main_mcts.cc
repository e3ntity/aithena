/*
Copyright 2020 All rights reserved.
*/

#include <stdlib.h>
#include <time.h>
#include <bitset>
#include <iomanip>
#include <memory>

#include "chess/game.h"
#include "chess/util.h"
#include "mcts/mcts.h"

int main(int argc, char** argv) {
  aithena::chess::Game::Options options = {{"board_width", 8},
                                           {"board_height", 8}};

  auto game = aithena::chess::Game(options);
  auto game_ptr = std::make_shared<aithena::chess::Game>(game);

  auto start = *aithena::chess::State::FromFEN(
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  aithena::Board board = start.GetBoard();

  std::cout << start.ToFEN() << std::endl;

  auto moves = game.GenMoves(start);

  std::cout << "Possible moves: " << moves.size() << std::endl;

  std::cout << PrintBoard(start) << std::endl;
  for (auto move : moves) {
    std::cout << move.ToFEN() << std::endl;
    aithena::BoardPlane markup = GetNewFields(move.GetBoard(), board);
    std::cout << PrintMarkedBoard(move, markup) << std::endl;
  }
}

/*
int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <rounds> <simulations>" << std::endl;
    return -1;
  }

  int rounds = std::stoi(argv[1]);
  int simulations = std::stoi(argv[2]);

  srand(time(NULL));

  aithena::chess::Game::Options options = {{"board_width", 5},
                                           {"board_height", 5},
                                           {"max_no_progress", 20},
                                           {"max_move_count", 20}};

  auto game = aithena::chess::Game(options);
  auto game_ptr = std::make_shared<aithena::chess::Game>(game);

  auto start = aithena::chess::State::FromFEN("4k/5/R4/RK3 w - - 0 1");

  auto root = aithena::MCTSNode<aithena::chess::Game>(game_ptr, *start);
  auto root_ptr =
      std::make_shared<aithena::MCTSNode<aithena::chess::Game>>(root);

  std::cout << PrintBoard(root_ptr->GetState()) << std::endl;

  auto mcts = aithena::MCTS<aithena::chess::Game>(game_ptr);

  std::shared_ptr<aithena::MCTSNode<aithena::chess::Game>> current_ptr =
      root_ptr;
  while (!game.IsTerminalState(current_ptr->GetState())) {
    train(mcts, current_ptr, rounds, simulations);

    current_ptr = mcts.GreedySelect(current_ptr);

    std::cout << PrintBoard(current_ptr->GetState()) << std::endl;
    std::cout << "Confidence: " << current_ptr->GetUCTConfidence() << std::endl;
  }

  std::cout << mcts.BenchmarkSelect() << " sec/select" << std::endl;
  std::cout << mcts.BenchmarkSimulate() << " sec/simulate" << std::endl;
  std::cout << mcts.BenchmarkBackpropagate() << " sec/backprop" << std::endl;
  std::cout << mcts.BenchmarkRun() << " sec/run" << std::endl;

  return 0;
}*/
