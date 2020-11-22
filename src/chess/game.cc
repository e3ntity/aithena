#include "chess/chess.h"

namespace aithena {
namespace chess {

Game::Game() : Game{Options{}} {}
/*
Game::Game(Options options) : ::aithena::Game{options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
}*/

State Game::GetInitialState() {
  return State();
}

Game::ActionList Game::GetLegalActions(State state) {
  return std::vector<Action>();
};

}
}
