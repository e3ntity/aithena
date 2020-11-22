#include "chess/chess.h"

#include <assert.h>

#include "chess/moves.h"

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
  State state(GetOption("board_width"), GetOption("board_height"),
              GetOption("figure_count"));


}

Game::ActionList Game::GetLegalActions(State state) {
  Game::ActionList legal_moves{};

  Game::ActionList pawn_moves = GenPawnMoves(state);
  legal_moves.reserve(legal_moves.size() + pawn_moves.size());
  legal_moves.insert(legal_moves.end(), pawn_moves.begin(), pawn_moves.end());

  return legal_moves;
};

}  // namespace chess
}  // namespace aithena
