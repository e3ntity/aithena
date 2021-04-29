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

  double nps = 1000 * nodes / b.GetLast(unit);

  std::cout << "Speed: " << nps << " nodes / sec" << std::endl;

  double total_time = game_ptr->benchmark_gen_moves_.GetAvg(unit);
  double find_king_time = game_ptr->benchmark_find_king_.GetAvg(unit);
  double danger_time = game_ptr->benchmark_danger_squares_.GetAvg(unit);
  double king_moves_time = game_ptr->benchmark_king_moves_.GetAvg(unit);
  double checks_time = game_ptr->benchmark_checks_.GetAvg(unit);
  double move_masks_time = game_ptr->benchmark_move_masks_.GetAvg(unit);
  double pin_moves_time = game_ptr->benchmark_pin_moves_.GetAvg(unit);
  double other_moves_time = game_ptr->benchmark_other_moves_.GetAvg(unit);

  std::cout << "Total time:  " << total_time << " usec" << std::endl;
  std::cout << "Find king:   " << find_king_time << " usec ("
            << 100 * find_king_time / total_time << "%)" << std::endl;
  std::cout << "Calc danger: " << danger_time << " usec ("
            << 100 * danger_time / total_time << "%)" << std::endl;
  std::cout << "King moves:  " << king_moves_time << " usec ("
            << 100 * king_moves_time / total_time << "%)" << std::endl;
  std::cout << "Checks:      " << checks_time << " usec ("
            << 100 * checks_time / total_time << "%)" << std::endl;
  std::cout << "Move masks:  " << move_masks_time << " usec ("
            << 100 * move_masks_time / total_time << "%)" << std::endl;
  std::cout << "Pin moves:   " << pin_moves_time << " usec ("
            << 100 * pin_moves_time / total_time << "%)" << std::endl;
  std::cout << "Other moves: " << other_moves_time << " usec ("
            << 100 * other_moves_time / total_time << "%)" << std::endl;

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
