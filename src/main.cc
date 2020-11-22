#include "chess/chess.h"

int main() {
  // Play 4x4 chess

  aithena::chess::Game::Options options = {
    {"board_width", 4},
    {"board_height", 4}
  };

  auto game = aithena::chess::Game(options);

  auto state = game.GetInitialState();
  auto actions = game.GetLegalActions(state);
  unsigned int user_action;


  while (true) {
    // Update screen

    // Have user select an action
    user_action = 5;

    //state = actions[user_action].GetNextState();
    actions = game.GetLegalActions(state);
  }
  return 0;
}
