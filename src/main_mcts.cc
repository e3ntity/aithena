/*
Copyright 2020 All rights reserved.
*/

#include <stdlib.h>
#include <time.h>
#include <bitset>
#include <iomanip>
#include <memory>

#include "benchmark/benchmark.h"
#include "chess/game.h"
#include "chess/util.h"
#include "mcts/mcts.h"

int unit = aithena::Benchmark::UNIT_USEC;

int main(int argc, char** argv) {
  aithena::chess::Game::Options options = {{"board_width", 8},
                                           {"board_height", 8}};

  auto game = aithena::chess::Game(options);
  auto game_ptr = std::make_shared<aithena::chess::Game>(game);

  auto start = *aithena::chess::State::FromFEN(
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

  aithena::Benchmark b;

  b.Start();

  int nodes = aithena::chess::perft(game_ptr, start, 3);

  b.End();

  double nps = 1000000.0 * static_cast<double>(nodes) /
               static_cast<double>(b.GetLast(unit));

  std::cout << "Speed: " << nps << " nodes / sec" << std::endl;

  std::cout << "## Game functions benchmark ##" << std::endl;

  for (auto time : game_ptr->benchmark_.GetAvg(unit)) {
    auto b = game_ptr->benchmark_.Get(std::get<0>(time));

    std::cout << std::get<0>(time) << ": " << std::get<1>(time) << " usec ("
              << b->GetSize() << " times called)" << std::endl;
  }

  std::cout << "## GenMoves benchmark ##" << std::endl;

  double total_time = game_ptr->benchmark_.Get("GenMoves()")->GetAvg(unit);
  auto timings = game_ptr->benchmark_gen_moves_.GetAvg(unit);

  for (auto time : timings) {
    std::cout << std::setprecision(2) << std::get<0>(time) << ": "
              << std::get<1>(time) << " usec ("
              << 100 * std::get<1>(time) / total_time << "%)" << std::endl;
  }

  return 0;
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
