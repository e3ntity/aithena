#include "benchmark.h"

#include <getopt.h>

#include <iostream>

#include "alphazero/alphazero.h"
#include "chess/game.h"
#include "chess/util.h"

using namespace aithena;

std::string GetBenchmarkUsageText() {
  return "Usage: ./aithena benchmark <options>\n"
         "  --help -h                       Show the help menu\n"
         "## Benchmark Options ##\n"
         "  --alphazero                     Run alphazero test\n"
         "  --divide <depth>                Run divide test (for locating bugs)\n"
         "  --perft <depth>                 Run perft test\n"
         "## AlphaZero Options ##\n"
         "  --no-cuda                       Disables using cuda\n"
         "  --simulations <number>          Number of simulations (default: 800)\n"
         "## Chess Options ##\n"
         "  --fen -f <string>               Initial board (default: 8-by-8 Chess)\n"
         "  --max-moves -m <number>         Maximum moves per game (default: 1000)\n"
         "  --max-no-progress -p <number>   Maximum moves without progress (default: 50)\n";
}

enum GetOptOption : int {
  kOptAlphazero = 1000,
  kOptDivide,
  kOptPerft,
  kOptNoCuda,
  kOptSimulations,
  kOptFEN,
  kOptMaxMoves,
  kOptMaxNoProgress
};

int RunBenchmark(int argc, char** argv) {
  static struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                         {"alphazero", no_argument, nullptr, kOptAlphazero},
                                         {"divide", required_argument, nullptr, kOptDivide},
                                         {"perft", required_argument, nullptr, kOptPerft},
                                         {"no-cuda", no_argument, nullptr, kOptNoCuda},
                                         {"simulations", required_argument, nullptr, kOptSimulations},
                                         {"fen", required_argument, nullptr, kOptFEN},
                                         {"max-moves", required_argument, nullptr, kOptMaxMoves},
                                         {"max-no-progress", required_argument, nullptr, kOptMaxNoProgress},
                                         {0, 0, 0, 0}};

  // Configurable options
  std::string fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  bool alphazero{false};
  int divide{-1};
  int az_rounds{1};  // TODO: make configurable
  bool az_no_cuda{false};
  int az_simulations{800};
  int perft{-1};
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
      case kOptAlphazero:
        alphazero = true;
        break;
      case kOptDivide:
        divide = atoi(optarg);
        break;
      case kOptPerft:
        perft = atoi(optarg);
        break;
      case kOptNoCuda:
        az_no_cuda = true;
        break;
      case kOptSimulations:
        az_simulations = atoi(optarg);
        break;
      case kOptFEN:
        fen = static_cast<std::string>(optarg);
        break;
      case kOptMaxMoves:
        max_moves = atoi(optarg);
        break;
      case kOptMaxNoProgress:
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

  Benchmark bm_bm;

  bm_bm.Start();

  if (perft >= 0) RunPerftBenchmark(game, start, perft);

  if (divide >= 0) RunDivide(game, start, divide);

  if (alphazero) RunAlphazeroBenchmark(game, start, az_simulations, az_rounds, az_no_cuda);

  bm_bm.End();

  std::cout << "Benchmark: completed in " << bm_bm.GetLast(Benchmark::UNIT_SEC) << " seconds" << std::endl;

  return 0;
}

void RunAlphazeroBenchmark(chess::Game::GamePtr game, chess::State::StatePtr state, int simulations,
                           int evaluation_games, bool no_cuda) {
  Benchmark bm_alphazero;

  bm_alphazero.Start();

  AlphaZero az{game};

  az.SetSimulations(simulations);

  if (no_cuda) az.SetUseCUDA(false);

  chess::State::StatePtr current_state = state;

  int action_count = 0;

  for (int i = 0; i < evaluation_games; ++i) {
    while (!game->IsTerminalState(current_state)) {
      current_state = az.DrawAction(current_state);
      ++action_count;
    }
  }

  bm_alphazero.End();

  double aps =
      1000000.0 * static_cast<double>(action_count) / static_cast<double>(bm_alphazero.GetLast(Benchmark::UNIT_USEC));

  std::cout << "## Alphazero ##" << std::endl;
  std::cout << "Drew " << action_count << " actions in " << bm_alphazero.GetLast(Benchmark::UNIT_SEC) << " seconds ("
            << aps << " actions per second)" << std::endl;

  for (auto bm : az.benchmark_.GetAvg(Benchmark::UNIT_MSEC))
    std::cout << std::get<0>(bm) << ": " << std::get<1>(bm) << " msec" << std::endl;
}

void RunPerftBenchmark(chess::Game::GamePtr game, chess::State::StatePtr state, int perft_depth) {
  Benchmark bm_perft;

  bm_perft.Start();

  int nodes = chess::perft(game, state, perft_depth);

  bm_perft.End();

  double nps = 1000000.0 * static_cast<double>(nodes) / static_cast<double>(bm_perft.GetLast(Benchmark::UNIT_USEC));

  std::cout << "## Perft(" << perft_depth << ") ##" << std::endl;
  std::cout << "Searched " << nodes << " nodes in " << bm_perft.GetLast(Benchmark::UNIT_SEC) << " seconds (" << nps
            << " nps)" << std::endl;
}

void RunDivide(chess::Game::GamePtr game, chess::State::StatePtr state, int depth) {
  Benchmark bm_divide;

  bm_divide.Start();

  auto divide = chess::divide(game, state, depth);

  bm_divide.End();

  int nodes = 0;
  for (auto entry : divide) nodes += std::get<1>(entry);

  double nps = 1000000.0 * static_cast<double>(nodes) / static_cast<double>(bm_divide.GetLast(Benchmark::UNIT_USEC));

  std::cout << "## Divide(" << depth << ") ##" << std::endl;
  std::cout << "Searched " << nodes << " nodes in " << bm_divide.GetLast(Benchmark::UNIT_SEC) << " seconds (" << nps
            << " nps)" << std::endl;

  for (auto entry : divide) std::cout << std::get<0>(entry)->ToLAN() << ": " << std::get<1>(entry) << std::endl;

  std::cout << "Moves: " << divide.size() << std::endl;
  std::cout << "Nodes: " << nodes << std::endl;
}
