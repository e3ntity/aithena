#include <iostream>

#include "chess/chess.h"

int main() {
  // Play 4x4 chess
  aithena::chess::Game::Options options = {
    {"board_width", 4},
    {"board_height", 4}
  };

  auto game = aithena::chess::Game(options);
  auto state = game.GetInitialState();

  aithena::Board board& = state.GetBoard();
  auto black_pawn = aithena::chess::make_piece(
                      aithena::chess::Figure::kPawn,
                      aithena::chess::Player::kBlack);
  board.SetPiece(0, 1, black_pawn);
  board.SetPiece(1, 1, black_pawn);
  board.SetPiece(3, 1, black_pawn);

  std::cout << "Initial State: " << state.ToString() << std::endl;

  /*
  auto actions = game.GetLegalActions(state);
  unsigned int user_action;

  //while (true) {
  // Update screen

  // Have user select an action
  user_action = 5;

  // Perform action
  state = actions[user_action].GetNextState();
  actions = game.GetLegalActions(state);
  //}
  */

  return 0;
}
