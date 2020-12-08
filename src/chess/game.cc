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

  // Pawns
  for (int i = 0; i < 8; ++i) {
    board.SetField(i, 1, make_piece(Figure::kPawn, Player::kWhite));
    board.SetField(i, 6, make_piece(Figure::kPawn, Player::kBlack));
  }

  // Rooks
  board.SetField(0, 0, make_piece(Figure::kRook, Player::kWhite));
  board.SetField(7, 0, make_piece(Figure::kRook, Player::kWhite));
  board.SetField(0, 7, make_piece(Figure::kRook, Player::kBlack));
  board.SetField(7, 7, make_piece(Figure::kRook, Player::kBlack));

  // Bishops
  board.SetField(2, 0, make_piece(Figure::kBishop, Player::kWhite));
  board.SetField(5, 0, make_piece(Figure::kBishop, Player::kWhite));
  board.SetField(2, 7, make_piece(Figure::kBishop, Player::kBlack));
  board.SetField(5, 7, make_piece(Figure::kBishop, Player::kBlack));

  // Knights
  board.SetField(1, 0, make_piece(Figure::kKnight, Player::kWhite));
  board.SetField(6, 0, make_piece(Figure::kKnight, Player::kWhite));
  board.SetField(1, 7, make_piece(Figure::kKnight, Player::kBlack));
  board.SetField(6, 7, make_piece(Figure::kKnight, Player::kBlack));

  // Kings
  board.SetField(3, 0, make_piece(Figure::kKing, Player::kWhite));
  board.SetField(3, 7, make_piece(Figure::kKing, Player::kBlack));

  // Queens
  board.SetField(4, 0, make_piece(Figure::kQueen, Player::kWhite));
  board.SetField(4, 7, make_piece(Figure::kQueen, Player::kBlack));
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

  if (x >= 1 && enemy_figures.get(x - 1, y + direction)) {
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

std::vector<State> Game::GenKingMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up + up + left, up + up + right, down + down + left, down + down + right},
  1);
}

std::vector<State> Game::GenQueenMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up + left, up + right, down + left, down + right, up, down, left, right}, 8);
}

std::vector<State> Game::GenKnightMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y, {
    up + up + left, up + up + right, down + down + left,
    down + down + right, up, down, left, right
  }, 1);
}

std::vector<State> Game::GenMoves(State state, bool pseudo) {
  Board board = state.GetBoard();
  unsigned width = board.GetWidth();
  unsigned height = board.GetHeight();

  std::vector<State> moves;

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      std::vector<State> piece_moves = GenMoves(state, x, y, pseudo);
      moves.reserve(moves.size() + piece_moves.size());
      moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
    }
  }

  return moves;
}

std::vector<State> Game::GenMoves(State state, unsigned x, unsigned y,
                                  bool pseudo) {
  Piece piece = state.GetBoard().GetField(x, y);
  std::vector<State> moves;

  // Make checks

  if (piece.player != static_cast<unsigned>(state.GetPlayer())
      || piece == kEmptyPiece)
    return moves;

  // Generate pseudo-moves

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
  case static_cast<unsigned>(Figure::kQueen):
    moves = GenQueenMoves(state, x, y);
    break;
  case static_cast<unsigned>(Figure::kKnight):
    moves = GenKnightMoves(state, x, y);
    break;
  case static_cast<unsigned>(Figure::kKing):
    moves = GenKingMoves(state, x, y);
    // TODO: generate king moves.
    break;
  case kEmptyPiece.figure:
    break;
  default:
    assert(false);
  }

  // Update player turn

  Player next_player = piece.player == static_cast<unsigned>(Player::kWhite)
                       ? Player::kBlack
                       : Player::kWhite;

  std::for_each(
    moves.begin(),
    moves.end(),
  [&next_player](State & s) { s.IncMoveCount(); s.SetPlayer(next_player); });

  if (pseudo) return moves;

  // Remove moves with king checked
  auto move = std::begin(moves);
  while (move != std::end(moves)) {
    if (KingInCheck(*move, state.GetPlayer()))
      move = moves.erase(move);
    else
      ++move;
  }

  return moves;
}

bool Game::KingInCheck(State state, Player player) {
  Board board = state.GetBoard();
  BoardPlane king_plane = board.GetPlane(make_piece(Figure::kKing, player));
  Player opponent = player == Player::kWhite ? Player::kBlack : Player::kWhite;
  std::vector<State> next_states = GenMoves(state, /*pseudo=*/ true);

  for (auto next_state : next_states) {
    if ((king_plane & next_state.GetBoard().GetPlayerPlane(opponent)).empty())
      continue;
    return true;
  }

  return false;
}

}  // namespace chess
}  // namespace aithena
