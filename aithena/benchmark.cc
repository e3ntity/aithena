#include <getopt.h>

#include <iostream>

#include "./main.h"
#include "chess/game.h"
#include "chess/util.h"

using namespace aithena;

std::string GetBenchmarkUsageText() {
  return "Usage: ./aithena benchmark <options>\n"
         "  --help -h                       Show the help menu\n"
         "## Benchmark Options ##\n"
         "  --depth -d <number>             Perft depth (default: 1)\n"
         "## Chess Options ##\n"
         "  --fen -f <string>               Initial board (default: 8-by-8 "
         "Chess)\n"
         "  --max-moves -m <number>         Maximum moves per game (default: "
         "1000)\n"
         "  --max-no-progress -p <number>   Maximum moves without progress "
         "(default: 50)\n";
}

int RunBenchmark(int argc, char** argv) {
  static struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                         {"depth", required_argument, nullptr, 'd'},
                                         {"fen", required_argument, nullptr, 'f'},
                                         {"max-moves", required_argument, nullptr, 'm'},
                                         {"max-no-progress", required_argument, nullptr, 'p'},
                                         {0, 0, 0, 0}};

  // Configurable options
  std::string fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  int perft_depth{1};
  int max_no_progress{50};
  int max_moves{1000};

  int long_index = 0;
  int opt = 0;
  while (true) {
    opt = getopt_long(argc, argv, "hd:f:m:p:", long_options, &long_index);

    if (opt == -1) break;

    switch (opt) {
      case 'h':
        std::cout << GetBenchmarkUsageText();
        return 0;
      case 'd':
        perft_depth = atoi(optarg);
        break;
      case 'f':
        fen = static_cast<std::string>(optarg);
        break;
      case 'm':
        max_moves = atoi(optarg);
        break;
      case 'p':
        max_no_progress = atoi(optarg);
        break;
      default:
        std::cout << GetBenchmarkUsageText();
        return 1;
    }
  }

  chess::State::StatePtr start = chess::State::FromFEN(fen);
  chess::Game::Options options = {{"board_width", start->GetBoard().GetWidth()},
                                  {"board_height", start->GetBoard().GetHeight()},
                                  {"max_no_progress", max_no_progress},
                                  {"max_move_count", max_moves}};
  chess::Game::GamePtr game = std::make_shared<chess::Game>(options);

  Benchmark bm_perft;
  Benchmark bm_bm;

  bm_bm.Start();

  bm_perft.Start();

  int nodes = chess::perft(game, start, perft_depth);

  bm_perft.End();

  double nps = 1000000.0 * static_cast<double>(nodes) / static_cast<double>(bm_perft.GetLast(Benchmark::UNIT_USEC));

  bm_bm.End();

  std::cout << "## Benchmark (completed in " << bm_bm.GetLast(Benchmark::UNIT_SEC) << " seconds) ##" << std::endl;
  std::cout << "Perft(" << perft_depth << "): Searched " << nodes << " nodes in "
            << bm_perft.GetLast(Benchmark::UNIT_SEC) << " seconds (" << nps << " nps)" << std::endl;

  return 0;
}