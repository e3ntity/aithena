#include "main.h"

#include <getopt.h>
#include <iostream>

#include "chess/game.h"
#include "chess/util.h"
#include "mcts/mcts.h"

using namespace aithena;

std::string GetMCTSUsageText() {
  return "Usage: ./aithena mcts <options>\n"
         "  --help -h                       Show the help menu\n"
         "## MCTS Options ##\n"
         "  --rounds -r <number>            Rounds to run for training the "
         "MCTS agent (default: 64)"
         "  --simulations -s <number>       Simulations to run per training "
         "round (default: 128)"
         "## Chess Options ##\n"
         "  --fen -f <string>               Initial board (default: 8-by-8 "
         "Chess)\n"
         "  --max-moves -m <number>         Maximum moves per game (default: "
         "1000)\n"
         "  --max-no-progress -p <number>   Maximum moves without progress "
         "(default: 50)\n";
}

int RunMCTS(int argc, char** argv) {
  static struct option long_options[] = {
      {"help", no_argument, nullptr, 'h'},
      {"rounds", required_argument, nullptr, 'r'},
      {"simulations", required_argument, nullptr, 's'},
      {"fen", required_argument, nullptr, 'f'},
      {"max-moves", required_argument, nullptr, 'm'},
      {"max-no-progress", required_argument, nullptr, 'p'},
      {0, 0, 0, 0}};

  // Configurable options
  int rounds = 64;
  int simulations = 128;
  std::string fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  int max_no_progress{50};
  int max_moves{1000};

  int long_index = 0;
  int opt = 0;
  while (true) {
    opt = getopt_long(argc, argv, "hr:s:f:m:p:", long_options, &long_index);

    if (opt == -1) break;

    switch (opt) {
      case 'h':
        std::cout << GetMCTSUsageText();
        return 0;
      case 'r':
        rounds = atoi(optarg);
        std::cout << "Rounds: " << rounds << std::endl;
        break;
      case 's':
        simulations = atoi(optarg);
        std::cout << "Simulations: " << simulations << std::endl;
        break;
      case 'f':
        fen = static_cast<std::string>(optarg);
        std::cout << "FEN: " << fen << std::endl;
        break;
      case 'm':
        max_moves = atoi(optarg);
        std::cout << "Max. moves: " << max_moves << std::endl;
        break;
      case 'p':
        max_no_progress = atoi(optarg);
        std::cout << "Max. no progress: " << max_no_progress << std::endl;
        break;
      default:
        std::cout << GetMCTSUsageText();
        return 1;
    }
  }

  // Setup chess
  chess::State::StatePtr start = chess::State::FromFEN(fen);
  chess::Game::Options options = {
      {"board_width", start->GetBoard().GetWidth()},
      {"board_height", start->GetBoard().GetHeight()},
      {"max_no_progress", max_no_progress},
      {"max_move_count", max_moves}};
  chess::Game::GamePtr game = std::make_shared<chess::Game>(options);

  // Setup MCTS
  MCTS<chess::Game> mcts = MCTS<chess::Game>(game);
  MCTSNode<chess::Game>::NodePtr node =
      std::make_shared<MCTSNode<chess::Game>>(game, *start);

  BenchmarkSet bm;

  bm.Start("total");

  while (!game->IsTerminalState(
      std::make_shared<chess::State>(node->GetState()))) {
    std::cout << PrintBoard(node->GetState()) << std::endl;

    for (int round = 0; round < rounds; ++round) {
      double progress =
          100 * static_cast<double>(round) / static_cast<double>(rounds);
      std::cout << "Running MCTS (" << static_cast<int>(progress) << "%)"
                << std::flush;

      mcts.Run(node, simulations);

      double remaining_rounds = rounds - round - 1;
      double sec_per_round = mcts.bm_.Get("Run")->GetAvg(Benchmark::UNIT_MSEC);
      std::cout << " remaining: "
                << static_cast<int>(remaining_rounds * sec_per_round / 1000)
                << " sec    \r" << std::flush;
    }

    node = mcts.GreedySelect(node);
  }

  bm.End("total");

  std::cout << PrintBoard(node->GetState()) << std::endl;

  std::cout << "Played complete game in "
            << bm.Get("total")->GetSum(Benchmark::UNIT_SEC) << " seconds"
            << std::endl;

  return 0;
}
