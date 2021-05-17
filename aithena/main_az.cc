/*
Copyright 2020 All rights reserved.
*/

#include <stdlib.h>
#include <time.h>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <memory>

#include "alphazero/alphazero.h"
#include "chess/game.h"
#include "chess/util.h"

#define NN_SAVE_PATH "./neural_net.pt"

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0] << " <rounds> <simulations> <batch size>"
              << std::endl;
    return -1;
  }

  int rounds = std::stoi(argv[1]);
  int simulations = std::stoi(argv[2]);
  int batch_size = std::stoi(argv[3]);

  // Setup game
  aithena::chess::Game::Options options = {{"board_width", 5},
                                           {"board_height", 5},
                                           {"max_no_progress", 20},
                                           {"max_move_count", 20}};

  auto game = aithena::chess::Game(options);
  auto game_ptr = std::make_shared<aithena::chess::Game>(game);

  aithena::alphazero::AZNeuralNetwork nn(5, 5, 5, torch::Device(torch::kCPU),
                                         true);
  aithena::alphazero::AlphaZero az(game_ptr, nn);

  az.SetDiscountFactor(0.99);

  // Load saved NN
  std::ifstream nn_file;
  nn_file.open(NN_SAVE_PATH);

  if (nn_file) {
    az.Load(NN_SAVE_PATH);
    std::cout << "Loaded neural network from \"" << NN_SAVE_PATH << "\""
              << std::endl;
  }

  nn_file.close();

  // Train AlphaZero
  auto root = aithena::alphazero::AZNode(game_ptr, nn);

  aithena::alphazero::AlphaZero::ReplayMemory mem;
  auto mem_ptr =
      std::make_shared<aithena::alphazero::AlphaZero::ReplayMemory>(mem);

  if (rounds > 0) {
    do {
      std::cout << "\rCollecting samples (" << mem_ptr->size() << "/"
                << batch_size << ")" << std::flush;
    } while (
        !az.Train(game.GetInitialState(), mem_ptr, batch_size, simulations));
  }

  std::cout << std::endl;

  for (int round = 0; round < rounds; ++round) {
    az.Train(game.GetInitialState(), mem_ptr, batch_size, simulations);
    az.Save(NN_SAVE_PATH);

    double remaining = az.BenchmarkTrain() * (rounds - round) / 60;
    std::cout << std::setprecision(2) << "#" << round + 1 << " [Value: "
              << nn->forward(root.GetNNInput())[0][-1].item<double>()
              << "] [Samples: " << mem_ptr->size() << "] [" << remaining
              << " min. Remaining]" << std::endl;
  }

  // Run a game with AlphaZero

  auto current_ptr = std::make_shared<aithena::alphazero::AZNode>(root);

  while (!current_ptr->IsTerminal()) {
    std::cout << PrintBoard(current_ptr->GetState()) << std::endl;

    az.SelfPlayGame(current_ptr, simulations);
    current_ptr = az.PUCTSelect(current_ptr);
  }

  std::cout << PrintBoard(current_ptr->GetState()) << std::endl;

  return 0;
}
