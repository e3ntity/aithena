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

#define NN_SAVE_PATH "./neural_net.pt"

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker,
                             std::string marker_color = "\033[31m") {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;
  std::vector<std::string> letters = {"A", "B", "C", "D", "E", "F", "G", "H"};

  std::string turn =
      state.GetPlayer() == aithena::chess::Player::kWhite ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1) ? "\033[1;47m" : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white =
          piece.player == static_cast<unsigned>(aithena::chess::Player::kWhite);
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
        repr << s_color << (marked ? marker_color + "- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? marker_color + "-" : " ") << s_color
             << s_piece << "\033[24m" << (marked ? marker_color + "-" : " ")
             << "\033[0m";
      }
    }
  }

  repr << std::endl << " ";
  for (int i = 0; i < int(board.GetWidth()); ++i) repr << "  " << letters.at(i);

  repr << std::endl;

  return repr.str();
}

std::string PrintBoard(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

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

  // Load saved NN
  std::ifstream nn_file;
  nn_file.open(NN_SAVE_PATH);

  if (nn_file) az.Load(NN_SAVE_PATH);

  nn_file.close();

  // Train AlphaZero
  auto root = aithena::alphazero::AZNode(game_ptr, nn);

  aithena::alphazero::AlphaZero::ReplayMemory mem;
  auto mem_ptr =
      std::make_shared<aithena::alphazero::AlphaZero::ReplayMemory>(mem);

  for (int round = 0; round < rounds; ++round) {
    az.Train(game.GetInitialState(), mem_ptr, batch_size, simulations);
    az.Save(NN_SAVE_PATH);

    std::cout << "Value for round " << round << ": "
              << nn->forward(root.GetNNInput())[0][-1].item<double>() << " ("
              << mem_ptr->size() << " samples"
              << ")" << std::endl;
  }

  // Run a game with AlphaZero

  auto current_ptr = std::make_shared<aithena::alphazero::AZNode>(root);

  while (!current_ptr->IsTerminal()) {
    std::cout << PrintBoard(current_ptr->GetState()) << std::endl;

    az.SelfPlayGame(current_ptr, simulations);
    std::cout << "done selfplay" << std::endl;
    current_ptr = az.PUCTSelect(current_ptr);
    std::cout << "done select" << std::endl;
  }

  std::cout << PrintBoard(current_ptr->GetState()) << std::endl;

  return 0;
}
