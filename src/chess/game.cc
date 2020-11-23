#include "chess/chess.h"

#include <assert.h>

#include "board/board.h"
#include "chess/moves.h"

namespace aithena {
namespace chess {

Game::Game() : Game{Options{}} {}

Game::Game(Options options) : ::aithena::Game<Action, State> {options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
  DefaultOption("figure_count", static_cast<unsigned>(Figure::kCount));

  assert(GetOption("board_width") <= 8 && GetOption("board_height") <= 8);
}

State Game::GetInitialState() {
  State state(GetOption("board_width"), GetOption("board_height"),
              GetOption("figure_count"));

  assert(GetOption("board_width") == 8 && GetOption("board_height") == 8);

  Board& board = state.GetBoard();
  auto white_pawn = aithena::chess::make_piece(
                      aithena::chess::Figure::kPawn,
                      aithena::chess::Player::kWhite);
  auto black_pawn = aithena::chess::make_piece(
                      aithena::chess::Figure::kPawn,
                      aithena::chess::Player::kBlack);

  board.SetField(1, 1, white_pawn);
  board.SetField(2, 1, white_pawn);
  board.SetField(5, 1, white_pawn);
  board.SetField(6, 1, white_pawn);
  board.SetField(1, 6, black_pawn);
  board.SetField(2, 6, black_pawn);
  board.SetField(5, 6, black_pawn);
  board.SetField(6, 6, black_pawn);

  return state;
}

Game::ActionList Game::GetLegalActions(State state) {
  Game::ActionList legal_moves{};

  Game::ActionList pawn_moves = GenPawnMoves(state);
  legal_moves.reserve(legal_moves.size() + pawn_moves.size());
  legal_moves.insert(legal_moves.end(), pawn_moves.begin(), pawn_moves.end());

  return legal_moves;
};

Piece make_piece(Figure figure, Player player) {
  return Piece{static_cast<unsigned>(figure), static_cast<unsigned>(player)};
};

}  // namespace chess
}  // namespace aithena
