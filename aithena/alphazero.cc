/**
 * Copyright (C) 2020 All Right Reserved
 */

#include "alphazero/alphazero.h"

#include <getopt.h>
#include <torch/torch.h>

#include <iomanip>
#include <iostream>
#include <string>

#include "./main.h"
#include "chess/game.h"
#include "chess/util.h"

using namespace aithena;

enum GetOptOption : int {
  kOptBatchSize = 1000,
  kOptSimulations,
  kOptSave,
  kOptLoad,
  kOptNoCuda,
  kOptFEN,
  kOptMaxMoves,
  kOptMaxNoProgress
};

std::string GetAlphazeroUsageText() {
  return "Usage: ./aithena alphazero <options>\n"
         "  --help -h                       Show the help menu\n"
         "## Alphazero Options ##\n"
         "  --batch-size <number>       Neural net. update batch size (default: 4096)\n"
         "  --epochs -e <number>        Number of epochs (default: 10)"
         "  --rounds -r <number>        Number of training rounds (default: 100)\n"
         "  --simulations <number>      Number of simulations (default: 800)\n"
         "  --save <path>               Path for saving NN (a suffix will be appended)\n"
         "  --load <path>               Path for loading NN (suffix will be appended)\n"
         "  --no-cuda                   Disables using cuda if available\n"
         "## Chess Options ##\n"
         "  --fen <string>              Initial board (default: 8-by-8 Chess)\n"
         "  --max-moves <number>        Maximum moves per game (default: 1000)\n"
         "  --max-no-progress <number>  Maximum moves without progress (default: 50)\n";
}

void PrintProgress(double progress = 0.0, int width = 80) {
  int bar_width = width - 5;
  int pos = static_cast<int>(round(bar_width * progress));

  std::cout << std::string(100, ' ') << "\r";

  std::cout << "[";
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos)
      std::cout << "#";
    else
      std::cout << " ";
  }

  std::cout << "] " << std::setw(4) << static_cast<int>(round(progress * 100.0)) << "%\r";
  std::cout.flush();
}

int RunAlphazero(int argc, char** argv) {
  static struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                         {"batch-size", required_argument, nullptr, kOptBatchSize},
                                         {"epochs", required_argument, nullptr, 'e'},
                                         {"rounds", required_argument, nullptr, 'r'},
                                         {"simulations", required_argument, nullptr, kOptSimulations},
                                         {"save", required_argument, nullptr, kOptSave},
                                         {"load", required_argument, nullptr, kOptLoad},
                                         {"no-cuda", no_argument, nullptr, kOptNoCuda},
                                         {"fen", required_argument, nullptr, kOptFEN},
                                         {"max-moves", required_argument, nullptr, kOptMaxMoves},
                                         {"max-no-progress", required_argument, nullptr, kOptMaxNoProgress},
                                         {0, 0, 0, 0}};

  // Configurable options
  int batch_size{4096};
  int epochs{10};
  int rounds{100};
  int simulations{800};
  std::string fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  int max_no_progress{50};
  int max_moves{1000};
  std::string save_path{""};
  std::string load_path{""};
  bool use_cuda{torch::cuda::cudnn_is_available()};

  int long_index = 0;
  int opt = 0;
  while (true) {
    opt = getopt_long(argc, argv, "he:r:", long_options, &long_index);

    if (opt == -1) break;

    switch (opt) {
      case 'h':
        std::cout << GetAlphazeroUsageText();
        return 0;
      case kOptBatchSize:
        batch_size = atoi(optarg);
        std::cout << "Batch size: " << batch_size << std::endl;
        break;
      case 'e':
        epochs = atoi(optarg);
        std::cout << "Epochs: " << epochs << std::endl;
        break;
      case 'r':
        rounds = atoi(optarg);
        std::cout << "Rounds: " << rounds << std::endl;
        break;
      case kOptSimulations:
        simulations = atoi(optarg);
        std::cout << "Simulations: " << simulations << std::endl;
        break;
      case kOptSave:
        save_path = static_cast<std::string>(optarg);
        std::cout << "Save path: " << save_path << std::endl;
        break;
      case kOptLoad:
        load_path = static_cast<std::string>(optarg);
        std::cout << "Load path: " << load_path << std::endl;
        break;
      case kOptNoCuda:
        use_cuda = false;
        std::cout << "Disabled using CUDA" << std::endl;
        break;
      case kOptFEN:
        fen = static_cast<std::string>(optarg);
        std::cout << "FEN: " << fen << std::endl;
        break;
      case kOptMaxMoves:
        max_moves = atoi(optarg);
        std::cout << "Max. moves: " << max_moves << std::endl;
        break;
      case kOptMaxNoProgress:
        max_no_progress = atoi(optarg);
        std::cout << "Max. no progress: " << max_no_progress << std::endl;
        break;
      default:
        std::cout << GetAlphazeroUsageText();
        return 1;
    }
  }

  chess::State::StatePtr state = chess::State::FromFEN(fen);
  chess::Game::GamePtr game =
      std::make_shared<chess::Game>(chess::Game::Options({{"board_width", state->GetBoard().GetWidth()},
                                                          {"board_height", state->GetBoard().GetHeight()},
                                                          {"max_move_count", max_moves},
                                                          {"max_no_progress", max_no_progress}}));
  AlphaZeroNet net = AlphaZeroNet(game);
  AlphaZero az{game, net};

  if (use_cuda) {
    std::cout << "Running on GPU!" << std::endl;
    az.SetUseCUDA(true);
  }

  if (!load_path.empty()) az.GetNetwork()->Load(load_path);

  az.GetReplayMemory()->SetMinSize(batch_size);
  az.GetReplayMemory()->SetMaxSize(batch_size + 5000);

  az.SetBatchSize(batch_size);
  az.SetSimulations(simulations);

  std::cout << "Running " << epochs << " epochs with " << rounds << " round on \"" << fen << "\"." << std::endl;

  Benchmark bn;

  for (int j = 0; j < epochs; ++j) {
    std::cout << "## Epoch " << j + 1 << "/" << epochs << " (at " << abs(bn.GetSum(Benchmark::UNIT_SEC)) << " sec) ##"
              << std::endl;

    bn.Start();

    for (int i = 0; i < rounds; ++i) {
      // std::cout << (az.GetReplayMemory()->IsReady() ? "Training " : "Gathering Samples ") << std::flush;

      // PrintProgress(static_cast<double>(i) / static_cast<double>(rounds));

      while (!az.SelfPlay(state)) ((void)0);

      // PrintProgress(static_cast<double>(i + 1) / static_cast<double>(rounds));
    }

    chess::State::StatePtr current_state = state;

    std::cout << "Evaluation: " << std::flush;
    std::vector<double> evaluations;
    while (!game->IsTerminalState(current_state)) {
      evaluations.push_back(az.EvaluateState(current_state, chess::Player::kWhite));
      current_state = az.DrawAction(current_state);

      std::cout << current_state->ToLAN() << " " << std::flush;
    }
    evaluations.push_back(az.EvaluateState(current_state, chess::Player::kWhite));

    int result = game->GetStateResult(current_state) * (current_state->GetPlayer() == state->GetPlayer() ? 1 : -1);
    std::cout << "- " << result << std::endl;

    std::cout << "Val: " << std::flush;
    for (auto evaluation : evaluations) std::cout << std::setw(4) << int(evaluation * 100) << "% " << std::flush;
    std::cout << std::endl;

    if (!save_path.empty()) az.GetNetwork()->Save(save_path);

    bn.End();
  }

  std::cout << "## Benchmark ##" << std::endl;

  for (auto entry : az.benchmark_.GetAvg(Benchmark::UNIT_MSEC)) {
    std::string name = std::get<0>(entry);
    int64_t time = std::get<1>(entry);

    std::cout << name << ": " << time << " msec" << std::endl;
  }

  return 0;
}
