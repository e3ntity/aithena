#include "chess/chess.h"

#include <assert.h>
#include <iostream>

#include "board/board.h"
#include "chess/moves.h"

namespace aithena {
namespace chess {

Game::Game() : Game{Options{}} {}

Game::Game(Options options) : ::aithena::Game<State> {options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
  DefaultOption("figure_count", static_cast<unsigned>(Figure::kCount));

  assert(GetOption("board_width") <= 8 && GetOption("board_height") <= 8);

  InitializeMagic();
}

void Game::InitializeMagic() {
  unsigned width = GetOption("board_width");
  unsigned height = GetOption("board_height");
  unsigned fig_count = static_cast<unsigned>(Figure::kCount);

  assert(width <= 8 && height <= 8);

  for (unsigned f = 0; f < fig_count; ++f) {
    magic_bit_planes_[f] = std::make_unique<BoardPlane[]>(width * height);
    magic_bit_planes_[f + 1] = std::make_unique<BoardPlane[]>(width * height);
    for (unsigned x = 0; x < width; ++x) {
      for (unsigned y = 0; y < height; ++y) {
        magic_bit_planes_[2 * f][y * width + x] =
          BoardPlane(kPawnPushes[y * 8 + x]);
        magic_bit_planes_[2 * f + 1][y * width + x] =
          BoardPlane(kPawnPushes[y * 8 + x]);
      }
    }
  }
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

std::vector<State> Game::GetLegalActions(State state) {
  return {};
};

Piece make_piece(Figure figure, Player player) {
  return Piece{static_cast<unsigned>(figure), static_cast<unsigned>(player)};
};

std::vector<State> Game::GenPawnMoves(State state, unsigned x, unsigned y) {
  Board board_before{state.GetBoard()};
  std::size_t width{board_before.GetWidth()};
  std::size_t height{board_before.GetHeight()};

  std::vector<State> moves{}; // Return value

  unsigned pawn = static_cast<unsigned>(Figure::kPawn);
  unsigned player = static_cast<unsigned>(state.GetPlayer());
  Piece piece = board_before.GetField(x, y);

  // Make checks

  if (piece == kEmptyPiece || piece.player != player || piece.figure != pawn)
    return moves;

  int direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  if (y + direction >= height || y + direction < 0) {
    // TODO: Add promotions
    return moves;
  }

  // Generate moves

  unsigned opponent = static_cast<unsigned>(
                        state.GetPlayer() == Player::kWhite
                        ? Player::kBlack : Player::kWhite);
  BoardPlane figure_plane = board_before.GetCompletePlane();

  // Generate pushes

  if (!figure_plane.get(x, y + direction)) {
    // Move forward once
    moves.push_back(State{state});
    moves.back().GetBoard().MoveField(x, y, x, y + direction);

    if (y == (state.GetPlayer() == Player::kWhite ? 1 : height - 2)
        && !figure_plane.get(x, y + 2 * direction)) {
      // Move forward twice
      moves.push_back(State{state});
      moves.back().GetBoard().MoveField(x, y, x, y + direction * 2);
    }
  }

  // Generate captures


  return moves;
}

/*
std::vector<State> Game::GenPawnMoves(State state, unsigned x, unsigned y) {
  Board curr_board = state.GetBoard();
  std::size_t width = curr_board.GetWidth();
  std::size_t height = curr_board.GetHeight();

  Piece piece = curr_board.GetField(x, y);
  unsigned pawn = static_cast<unsigned>(Figure::kPawn);
  unsigned curr_player = static_cast<unsigned>(state.GetPlayer());
  signed direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  // Check if there is a pawn on the given (x, y) position.
  if (!(piece.figure == pawn
        && piece.player == curr_player)
      || y + direction >= height || y + direction < 0)
    return {};  // TODO: promotions

  std::vector<State> result;

  // Get enemy player
  Player enemy = state.GetPlayer() == Player::kWhite ?
                 Player::kBlack : Player::kWhite;

  BoardPlane pushes = magic_bit_planes_[pawn][y * width + x];
  BoardPlane attacks = magic_bit_planes_[pawn + 1][y * width + x];
  Piece own_pawn{pawn, curr_player};
  BoardPlane own_figures = curr_board.GetPlayerPlane(own_pawn);
  Piece enemy_pawn{pawn, static_cast<unsigned>(enemy)};
  BoardPlane enemy_figures = curr_board.GetPlayerPlane(enemy_pawn);

  BoardPlane push_collisions = (own_figures | enemy_figures) & pushes;
  if (!push_collisions.get(x, y + direction)) {
    State new_state = state;
    new_state.GetBoard().MoveField(x, y, x, y + direction);
    result.push_back(new_state);
  }

  BoardPlane attackable = enemy_figures & attacks;
  if (x + 1 < width) {
    if (attackable.get(x + 1, y + direction)) {
      State new_state = state;
      new_state.GetBoard().MoveField(x, y, x + 1, y + direction);
      result.push_back(new_state);
    }
  }
  if (x - 1 < width) {
    if (attacks.get(x + 1, y + direction)) {
      State new_state = state;
      new_state.GetBoard().MoveField(x, y, x - 1, y + direction);
      result.push_back(new_state);
    }
  }
  return result;
}
*/

std::vector<State> Game::GenMoves(State state, unsigned x, unsigned y) {
  return {};
}

}  // namespace chess
}  // namespace aithena
