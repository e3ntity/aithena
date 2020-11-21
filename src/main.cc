#include "chess/chess.h"

int main() {
  // Play 4x4 chess
  aithena::chess::Game::Options options = {
    {"board_width", 4},
    {"board_height", 4}
  };

  aithena::chess::Game game(options);

  //auto state = game.GetInitialState();
  //auto actions = game.GetLegalActions();
  //unsigned int user_action;

  //while (true/*state.IsUndecided()*/) {
  // Update screen

  // Have user select an action
  //user_action = 5;
  //state = actions[user_action].GetNextState();
  //}

  return 0;
}
