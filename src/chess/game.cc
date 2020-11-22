#include <assert.h>

#include "chess/chess.h"

namespace aithena {
namespace chess {

Game::Game() : Game{Options{}} {}

Game::Game(Options options) : ::aithena::Game<Action, State> {options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
  DefaultOption("figure_count", static_cast<unsigned char>(Figure::kCount));

  assert((
    "Chess board can at most be 8x8",
    GetOption("board_width") <= 8 && GetOption("board_height") <= 8
  ));
}

State Game::GetInitialState() {
  return State(GetOption("board_width"), GetOption("board_height"),
               GetOption("figure_count"));
}

Game::ActionList Game::GetLegalActions(State state) {
  return std::vector<Action>();
};

}  // namespace chess
}  // namespace aithena
