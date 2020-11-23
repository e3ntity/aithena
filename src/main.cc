#include <cstdlib>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "chess/chess.h"

void PrintHelp() {
  std::cout << "Possible actions:" << std::endl;
  std::cout << "  start                - Starts a new game." << std::endl;
  std::cout << "  help                 - Displays this menu." << std::endl;
  std::cout << "  exit                 - Exits AIthena." << std::endl;
  std::cout << "  show                 - Displays the board." << std::endl;
  std::cout << "  info <field>         - Show possible moves for a piece."
            << std::endl;
  std::cout << "  move <field> <field> - Move a piece from the first to the second field."
            << std::endl;
}

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker) {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << " " << std::string(board.GetWidth() * 4 + 1, '-')
         << std::endl << y << "|";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      piece = board.GetField(x, y);
      bool marked = marker.get(x, y);

      if (piece == aithena::kEmptyPiece) {
        repr << (marked ? "- -|" : "   |");
      } else {
        repr << (marked ? "-" : " ") << piece.figure << (marked ? "-|" : " |");
      }
    }
  }
  repr << std::endl << " " << std::string(board.GetWidth() * 4 + 1, '-')
       << std::endl << "   A   B   C   D   E   F   G   H" << std::endl;

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

  if (x > 96 && x < 123) x -= 97;
  else if (x > 64 && x < 91) x -= 65;
  else return std::make_tuple(0, 0, false);

  if (y > 47 && y < 58) y -= 48;
  else return std::make_tuple(0, 0, false);

  return std::make_tuple(x, y, true);
}

template <typename Game, typename State>
State EvaluateUserInput(std::string input, Game& game, State& state) {
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
  }
  // Show command
  else if (input_parts[0].compare("show") == 0) {
    std::cout << PrintBoard(state) << std::endl;

    return state;
  }
  // Info command
  else if (input_parts[0].compare("info") == 0) {
    if (input_parts.size() != 2) {
      std::cout << "Incorrect usage of \"info\"" << std::endl;
      PrintHelp();
      return state;
    }

    auto field = ParseField(input_parts[1]);
    auto moves = game.GenPawnMoves(state, std::get<0>(field),
                                   std::get<1>(field));
    aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

    for (auto move : moves)
      marker |= BoardDifference(board, move.GetBoard()).GetCompletePlane();

    std::cout << PrintMarkedBoard(state, marker) << std::endl;

    return state;
  }
  // Move command
  else if (input_parts[0].compare("move") == 0) {

  }

  return state;
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
  }

  return 0;
}
