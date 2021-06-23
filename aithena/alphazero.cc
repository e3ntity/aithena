/**
 * Copyright (C) 2020 All Right Reserved
 */

#include "alphazero.h"

#include <getopt.h>
#include <torch/torch.h>

#include <iomanip>
#include <iostream>
#include <string>

#include "alphazero/alphazero.h"
#include "chess/game.h"
#include "chess/util.h"
#include "mcts/mcts.h"

using namespace aithena;

enum GetOptOption : int {
  kOptEvaluate = 1000,
  kOptTrain,
  kOptEvaluations,
  kOptMCTSSimulations,
  kOptBatchSize,
  kOptSimulations,
  kOptSave,
  kOptDiscountFactor,
  kOptLoad,
  kOptNoCuda,
  kOptFEN,
  kOptMaxMoves,
  kOptMaxNoProgress
};

std::string GetAlphazeroUsageText() {
  return "Usage: ./aithena alphazero <options>\n"
         "  --evaluate                  Evaluate agent against MCTS (mutually exclusive with --train)\n"
         "  --help -h                   Show the help menu\n"
         "  --train                     Puts the program in training mode\n"
         "## Evaluation Options ##\n"
         "  --evaluations <number>      Number of evaluations to run (default: 10)\n"
         "  --mcts-simulations <number> Number of MCTS simulations (default: " +
         std::to_string(MCTS::kDefaultSimulations) +
         ")\n"
         "## Training Options ##\n"
         "  --batch-size <number>       Neural net. update batch size (default: " +
         std::to_string(AlphaZero::kDefaultBatchSize) +
         ")\n"
         "  --epochs -e <number>        Number of epochs (default: 10)\n"
         "  --rounds -r <number>        Number of training rounds (default: 100)\n"
         "  --save <path>               Path for saving NN (a suffix will be appended)\n"
         "## Alphazero Options ##\n"
         "  --discount-factor <number>  Discount factor (default: " +
         std::to_string(AlphaZero::kDefaultDiscountFactor) +
         ")\n"
         "  --load <path>               Path for loading NN (suffix will be appended)\n"
         "  --no-cuda                   Disables using cuda\n"
         "  --simulations <number>      Number of simulations (default: " +
         std::to_string(AlphaZero::kDefaultSimulations) +
         ")\n"
         "## Chess Options ##\n"
         "  --fen <string>              Initial board (default: 8-by-8 Chess)\n"
         "  --max-moves <number>        Maximum moves per game (default: 1000)\n"
         "  --max-no-progress <number>  Maximum moves without progress (default: 50)\n";
}

void ClearLine() { std::cout << std::string(100, ' ') << "\r"; }

void PrintProgress(double progress = 0.0, int width = 80, std::string prefix = "", std::string suffix = "") {
  int bar_width = std::max<int>(width - 8 - prefix.length() - suffix.length(), 10);
  int pos = static_cast<int>(round(bar_width * progress));

  ClearLine();

  std::cout << prefix << "[";
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos)
      std::cout << "#";
    else
      std::cout << " ";
  }

  std::cout << "] " << std::setw(4) << static_cast<int>(round(progress * 100.0)) << "%" << suffix << "\r";
  std::cout.flush();
}

int Evaluate(std::shared_ptr<AlphaZero> az, chess::State::StatePtr state,
             int mcts_simulations = MCTS::kDefaultSimulations) {
  chess::Game::GamePtr game = az->GetGame();
  MCTS mcts(game);
  mcts.SetSimulations(mcts_simulations);
  chess::State::StatePtr current_state = state;

  std::cout << "Evaluation: " << std::flush;
  while (!game->IsTerminalState(current_state)) {
    if (current_state->GetPlayer() == state->GetPlayer())
      current_state = az->DrawAction(current_state);
    else
      current_state = mcts.DrawAction(current_state);

    std::cout << current_state->ToLAN() << " " << std::flush;
  }

  int result = game->GetStateResult(current_state) * (current_state->GetPlayer() == state->GetPlayer() ? 1 : -1);
  std::cout << "- " << result << std::endl;

  return result;
}

int RunAlphazero(int argc, char** argv) {
  static struct option long_options[] = {{"evaluate", no_argument, nullptr, kOptEvaluate},
                                         {"help", no_argument, nullptr, 'h'},
                                         {"train", no_argument, nullptr, kOptTrain},
                                         {"mcts-simulations", required_argument, nullptr, kOptMCTSSimulations},
                                         {"batch-size", required_argument, nullptr, kOptBatchSize},
                                         {"epochs", required_argument, nullptr, 'e'},
                                         {"evaluations", required_argument, nullptr, kOptEvaluations},
                                         {"rounds", required_argument, nullptr, 'r'},
                                         {"save", required_argument, nullptr, kOptSave},
                                         {"discount-factor", required_argument, nullptr, kOptDiscountFactor},
                                         {"load", required_argument, nullptr, kOptLoad},
                                         {"no-cuda", no_argument, nullptr, kOptNoCuda},
                                         {"simulations", required_argument, nullptr, kOptSimulations},
                                         {"fen", required_argument, nullptr, kOptFEN},
                                         {"max-moves", required_argument, nullptr, kOptMaxMoves},
                                         {"max-no-progress", required_argument, nullptr, kOptMaxNoProgress},
                                         {0, 0, 0, 0}};

  // Configurable options
  int batch_size{AlphaZero::kDefaultBatchSize};
  int epochs{10};
  int rounds{100};
  int evaluations{10};
  int simulations{AlphaZero::kDefaultSimulations};
  double discount_factor{AlphaZero::kDefaultDiscountFactor};
  std::string fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  int max_no_progress{50};
  int max_moves{1000};
  std::string save_path{""};
  std::string load_path{""};
  bool use_cuda{torch::cuda::cudnn_is_available()};
  bool evaluate_mode{false};
  bool training_mode{false};
  int mcts_simulations{MCTS::kDefaultSimulations};

  int long_index = 0;
  int opt = 0;
  while (true) {
    opt = getopt_long(argc, argv, "he:r:", long_options, &long_index);

    if (opt == -1) break;

    switch (opt) {
      case kOptEvaluate:
        if (training_mode) {
          std::cout << "--train and --full are mutually exclusive" << std::endl;
          return 2;
        }
        evaluate_mode = true;
        std::cout << "Full mode enabled" << std::endl;
        break;
      case 'h':
        std::cout << GetAlphazeroUsageText();
        return 0;
      case kOptTrain:
        if (evaluate_mode) {
          std::cout << "--train and --full are mutually exclusive" << std::endl;
          return 2;
        }
        training_mode = true;
        std::cout << "Training mode enabled" << std::endl;
        break;
      case kOptMCTSSimulations:
        mcts_simulations = atoi(optarg);
        std::cout << "MCTS simulations: " << mcts_simulations << std::endl;
        break;
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
      case kOptEvaluations:
        evaluations = atoi(optarg);
        std::cout << "Evaluations: " << evaluations << std::endl;
        break;
      case kOptSimulations:
        simulations = atoi(optarg);
        std::cout << "Simulations: " << simulations << std::endl;
        break;
      case kOptSave:
        save_path = static_cast<std::string>(optarg);
        std::cout << "Save path: " << save_path << std::endl;
        break;
      case kOptDiscountFactor:
        discount_factor = atof(optarg);
        std::cout << "Discount factor: " << discount_factor << std::endl;
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
  az.SetDiscountFactor(discount_factor);

  if (evaluate_mode) {
    chess::State::StatePtr current_state = state;

    for (int i = 0; i < evaluations; ++i) Evaluate(std::make_shared<AlphaZero>(az), state, mcts_simulations);

    return 0;
  }

  if (!training_mode) {
    chess::State::StatePtr move = az.DrawAction(state);

    std::cout << "Selected move: " << move->ToLAN() << " (" << az.EvaluateState(move, state->GetPlayer()) << ")"
              << std::endl;
    std::cout << "Resulting in: " << move->ToFEN() << std::endl;

    return 0;
  }

  Benchmark bn;

  while (!az.GetReplayMemory()->IsReady()) {
    bn.Start();

    double sample_count = static_cast<double>(az.GetReplayMemory()->GetSampleCount());
    double min_sample_count = static_cast<double>(az.GetReplayMemory()->GetMinSize());

    PrintProgress(sample_count / min_sample_count, 80, "Gathering Samples ");

    az.SelfPlay(state);

    bn.End();
  }

  for (int j = 0; j < epochs; ++j) {
    ClearLine();
    std::cout << "## Epoch " << j + 1 << "/" << epochs << " (at " << abs(bn.GetSum(Benchmark::UNIT_SEC)) << " sec) ##"
              << std::endl;

    bn.Start();

    for (int i = 0; i < rounds; ++i) {
      PrintProgress(static_cast<double>(i) / static_cast<double>(rounds), 80, "Training ");

      az.SelfPlay(state);

      az.TrainNetwork();
    }

    double total_evaluation = 0;
    for (int i = 0; i < evaluations; ++i) {
      int result = Evaluate(std::make_shared<AlphaZero>(az), state, mcts_simulations);

      total_evaluation += result;
    }

    std::cout << "Evaluation average: " << total_evaluation / static_cast<double>(evaluations) << std::endl;

    if (!save_path.empty()) az.GetNetwork()->Save(save_path);

    bn.End();
  }

  return 0;
}
