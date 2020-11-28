#include "chess/game.h"

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

  //InitializeMagic();
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

  board.SetField(1, 1, aithena::chess::make_piece(
                   aithena::chess::Figure::kBishop,
                   aithena::chess::Player::kWhite));
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
  auto opponent = state.GetPlayer() == Player::kWhite
                  ? Player::kBlack : Player::kWhite;
  BoardPlane enemy_figures = board_before.GetPlayerPlane(
                               static_cast<unsigned>(opponent)
                             );

  if (x + 1 < width && enemy_figures.get(x + 1, y + direction)) {
    moves.push_back(State{state});
    moves.back().GetBoard().MoveField(x, y, x + 1, y + direction);
  }

  if (x - 1 >= 0 && enemy_figures.get(x - 1, y + direction)) {
    moves.push_back(State{state});
    moves.back().GetBoard().MoveField(x, y, x - 1, y + direction);
  }

  return moves;
}

std::vector<State> Game::GenRookMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y, {up, down, left, right}, 8);
}

std::vector<State> Game::GenBishopMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up + left, up + right, down + left, down + right}, 8);
}

std::vector<State> Game::GenMoves(State state, unsigned x, unsigned y) {
  Piece piece = state.GetBoard().GetField(x, y);
  std::vector<State> moves;

  switch (piece.figure) {
  case static_cast<unsigned>(Figure::kPawn):
    moves = GenPawnMoves(state, x, y);
    break;
  case static_cast<unsigned>(Figure::kRook):
    moves = GenRookMoves(state, x, y);
    break;
  case static_cast<unsigned>(Figure::kBishop):
    moves = GenBishopMoves(state, x, y);
    break;
  case kEmptyPiece.figure:
    break;
  default:
    assert(false);
  }

  Player next_player = piece.player == static_cast<unsigned>(Player::kWhite)
                       ? Player::kBlack
                       : Player::kWhite;

  std::for_each(
    moves.begin(),
    moves.end(),
  [&next_player](State & s) { s.IncMoveCount(); s.SetPlayer(next_player); });

  return moves;
}

}  // namespace chess
}  // namespace aithena
