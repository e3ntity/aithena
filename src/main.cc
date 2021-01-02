/*
Copyright 2020 All rights reserved.
*/

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "chess/game.h"
#include "mcts/mcts.h"

void PrintHelp() {
  std::cout << "Possible actions:" << std::endl;
  std::cout << "  start                 - Starts a new game." << std::endl;
  std::cout << "  help                  - Displays this menu." << std::endl;
  std::cout << "  exit                  - Exits AIthena." << std::endl;
  std::cout << "  show                  - Displays the board." << std::endl;
  std::cout << "  info <field>          - Show possible moves for a piece."
            << std::endl;
  std::string moves_inst = "  move <field> <field>";
  std::string moves_desc = "  - Move a piece from first to second field.";
  std::cout << moves_inst + moves_desc << std::endl;

  std::string promote_inst = "  promote <field> <fig>";
  std::string promote_desc = " - Promotes a pawn on field to figure fig.";
  std::cout << promote_inst + promote_desc << std::endl;
  std::cout << "  suggest               - Let the engine suggest a move"
            << std::endl;
  std::cout << "  benchmark             - Benchmark chess engine" << std::endl;
}

template <typename Game>
void Benchmark(Game game) {
  aithena::MCTSNode<aithena::chess::Game> root{game, game.GetInitialState(), nullptr};
  aithena::MCTS<aithena::chess::Game> mcts{game};
  auto root_ptr = std::make_shared<aithena::MCTSNode<aithena::chess::Game>>(root);

  // Settings
  int sim_runs = 10;

  // Take measurements

  auto sim_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < sim_runs; i++)
    mcts.Simulate(root_ptr, mcts.RandomSelect);
  auto sim_end = std::chrono::high_resolution_clock::now();

  auto sim_time = std::chrono::duration_cast<std::chrono::seconds>(sim_end - sim_start).count();
  auto sim_time_each = static_cast<double>(sim_time) / static_cast<double>(sim_runs);

  // Collect results
  std::cout << "Simulating took " << sim_time_each << " seconds per run."
            << std::endl;
}

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker) {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;

  std::string turn = state.GetPlayer() == aithena::chess::Player::kWhite
                     ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1)
                            ? "\033[1;47m"
                            : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white = piece.player ==
                   static_cast<unsigned>(aithena::chess::Player::kWhite);
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
        repr << s_color << (marked ? "\033[31m- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? "\033[31m-" : " ") << s_color << s_piece
             << "\033[24m" << (marked ? "\033[31m-" : " ") << "\033[0m";
      }
    }
  }
  repr << std::endl << "   A  B  C  D  E  F  G  H" << std::endl;

  return repr.str();
}

std::string PrintBoard(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

// Parses a string of format "A3"/"a3" to a position 1-3.
// Returns false as the third tuple entry if the received position was invalid.
std::tuple<unsigned, unsigned, bool> ParseField(std::string field) {
  if (field.length() != 2) return std::make_tuple(0, 0, false);

  unsigned x = unsigned(field[0]);
  unsigned y = unsigned(field[1]);

  if (x > 96 && x < 123)
    x -= 97;
  else if (x > 64 && x < 91)
    x -= 65;
  else
    return std::make_tuple(0, 0, false);

  if (y > 47 && y < 58)
    y -= 49;
  else
    return std::make_tuple(0, 0, false);

  return std::make_tuple(x, y, true);
}

template <typename Game, typename State>
State EvaluateUserInput(const std::string input,
                        Game& game, State& state) {
  if (input.compare("help") == 0) {
    PrintHelp();
    return state;
  }

  if (input.compare("exit") == 0) {
    std::cout << "Bye!" << std::endl;
    std::exit(0);
  }

  aithena::Board board = state.GetBoard();
  std::vector<std::string> input_parts;
  boost::split(input_parts, input, boost::is_any_of(" "));

  // Start command
  if (input_parts[0].compare("start") == 0) {
    aithena::chess::State new_state = game.GetInitialState();

    std::cout << PrintBoard(state) << std::endl;

    return new_state;
  } else if (input_parts[0].compare("show") == 0) {
    std::cout << PrintBoard(state) << std::endl;

    return state;
  } else if (input_parts[0].compare("info") == 0) {
    if (input_parts.size() != 2) {
      std::cout << "Incorrect usage of \"info\"" << std::endl;
      PrintHelp();

      return state;
    }

    auto field = ParseField(input_parts[1]);
    auto piece = board.GetField(std::get<0>(field), std::get<1>(field));

    if (piece == aithena::kEmptyPiece) {
      std::cout << "No piece at " << input_parts[1] << std::endl;
      return state;
    }

    auto moves = game.GenMoves(state, std::get<0>(field), std::get<1>(field));
    aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

    for (auto move : moves) {
      marker |= aithena::GetNewFields(board, move.GetBoard());
    }
    marker.clear(std::get<0>(field), std::get<1>(field));

    std::cout << PrintMarkedBoard(state, marker) << std::endl;

    return state;
  } else if (input_parts[0].compare("move") == 0) {
    if (input_parts.size() != 3) {
      std::cout << "Incorrect usage of \"move\"" << std::endl;
      PrintHelp();

      return state;
    }

    auto source = ParseField(input_parts[1]);
    auto target = ParseField(input_parts[2]);

    auto piece = board.GetField(std::get<0>(source), std::get<1>(source));

    if (piece == aithena::kEmptyPiece) {
      std::cout << "No piece at " << input_parts[1] << std::endl;
      return state;
    }

    auto move_board = state.GetBoard();
    move_board.MoveField(std::get<0>(source), std::get<1>(source),
                         std::get<0>(target), std::get<1>(target));

    if (piece.figure == static_cast<unsigned>(aithena::chess::Figure::kPawn)) {
      if (
        static_cast<unsigned>(state.GetDPushPawnX()) == std::get<0>(target)
        && (
          static_cast<unsigned>(state.GetDPushPawnY() % 8)
          == std::get<1>(target))) {
        move_board.ClearField(
          std::get<0>(target),
          static_cast<unsigned>(state.GetDPushPawnY() / 8));
      }
    }

    auto moves = game.GenMoves(state, std::get<0>(source), std::get<1>(source));
    unsigned i;

    for (i = 0; i < moves.size(); ++i) {
      if (move_board == moves[i].GetBoard()) break;
    }

    if (i >= moves.size()) {
      std::cout << "Illegal move" << std::endl;
      return state;
    }

    std::cout << PrintBoard(moves[i]) << std::endl;

    return moves[i];
  } else if (input_parts[0].compare("promote") == 0) {
    if (input_parts.size() != 3) {
      std::cout << "Incorrect usage of \"promote\"" << std::endl;
      PrintHelp();

      return state;
    }

    auto field = ParseField(input_parts[1]);
    auto piece = board.GetField(std::get<0>(field), std::get<1>(field));

    if (piece.figure != static_cast<unsigned>(aithena::chess::Figure::kPawn)) {
      std::cout << "No pawn at " << input_parts[1] << std::endl;
    }

    aithena::chess::Figure next_figure;

    switch (std::tolower(input_parts[2].front())) {
    case 'q':
      next_figure = aithena::chess::Figure::kQueen;
      break;
    case 'r':
      next_figure = aithena::chess::Figure::kRook;
      break;
    case 'n':
      next_figure = aithena::chess::Figure::kKnight;
      break;
    case 'b':
      next_figure = aithena::chess::Figure::kBishop;
      break;
    default:
      std::cout << "Invalid figure \"" << input_parts[2] << "\"" << std::endl;
      return state;
    }

    aithena::Piece next_piece = make_piece(next_figure, state.GetPlayer());
    aithena::Board next_board = state.GetBoard();

    next_board.SetField(std::get<0>(field), std::get<1>(field), next_piece);

    auto moves = game.GenMoves(state, std::get<0>(field), std::get<1>(field));
    unsigned i;

    for (i = 0; i < moves.size(); ++i) {
      if (next_board == moves[i].GetBoard()) break;
    }

    if (i >= moves.size()) {
      std::cout << "Illegal promotion" << std::endl;
      return state;
    }

    return moves[i];
  } else if (input_parts[0].compare("suggest") == 0) {
    aithena::MCTSNode<aithena::chess::Game> root{game, state, nullptr};
    aithena::MCTS<aithena::chess::Game> mcts{game};
    auto root_ptr = std::make_shared<aithena::MCTSNode<aithena::chess::Game>>(root);

    mcts.Run(root_ptr, 25, 10);

    auto next = mcts.GreedySelect(root_ptr);
    auto marker = aithena::GetNewFields(board, next->GetState().GetBoard());

    std::cout << PrintMarkedBoard(state, marker) << std::endl;

    std::cout << "Selection: " << mcts.BenchmarkSelect() << std::endl;
    std::cout << "Simulation: " << mcts.BenchmarkSimulate() << std::endl;
    std::cout << "Backpropagation: " << mcts.BenchmarkBackpropagate() << std::endl;

    return state;
  } else if (input_parts[0].compare("benchmark") == 0) {
    Benchmark(game);

    return state;
  }

  return state;
}

template <typename Game, typename State>
bool CheckWinner(Game& game, State& state) {
  auto next_moves = game.GenMoves(state);

  if (next_moves.size() > 0) return false;

  std::string winner = state.GetPlayer() == aithena::chess::Player::kWhite
                       ? "Black" : "White";

  std::cout << std::endl << std::endl << std::endl << winner
            << " is victorious!" << std::endl << std::endl << std::endl;

  return true;
}

int main() {
  aithena::chess::Game::Options options = {
    {"board_width", 8},
    {"board_height", 8}
  };

  aithena::chess::Game game = aithena::chess::Game(options);
  aithena::chess::State state = game.GetInitialState();

  std::string user_input;

  while (true) {
    // Have user select an action
    std::cout << "aithena > ";
    getline(std::cin, user_input);

    // Perform action
    state = EvaluateUserInput(user_input, game, state);
    CheckWinner(game, state);
  }

  return 0;
}
