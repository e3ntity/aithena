#include <iostream>

#include "chess/chess.h"

template <typename Game, typename State>
State EvaluateUserInput(std::string input, Game game, State& state) {
  if (input.compare("help") == 0) {
    std::cout << "Possible actions:" << std::endl;
    std::cout << "  start                - Starts a new game." << std::endl;
    std::cout << "  help                 - Displays this menu." << std::endl;
    std::cout << "  info <field>         - Show possible moves for a piece."
              << std::endl;
    std::cout << "  move <field> <field> - Move a piece from the first to the second field."
              << std::endl;

    return state;
  }

  if (input.compare("start") == 0) {
    aithena::chess::State new_state = game.GetInitialState();

    std::cout << state.PrintBoard() << std::endl;

    return new_state;
  } else if (input.compare("info") == 0) {

  } else if (input.compare("move") == 0) {

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
