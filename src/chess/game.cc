/*
Copyright 2020 All rights reserved.
*/

#include "chess/game.h"

#include "board/board.h"
#include "chess/moves.h"

namespace aithena {
namespace chess {

// Functions

Piece make_piece(Figure figure, Player player) {
  return Piece{static_cast<unsigned>(figure), static_cast<unsigned>(player)};
}

// Constructors

Game::Game() : Game{Options{}} {}

Game::Game(Options options) : ::aithena::Game<State> {options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
  DefaultOption("max_no_progress", 30);
  DefaultOption("max_move_count", 80);
  DefaultOption("figure_count", static_cast<unsigned>(Figure::kCount));

  max_no_progress_ = GetOption("max_no_progress");
  max_move_count_ = GetOption("max_move_count");

  assert(GetOption("board_width") <= 8 && GetOption("board_height") <= 8);

  // InitializeMagic();
}

Game::Game(const Game& other) : ::aithena::Game<State>(other) {
  // InitializeMagic();
}

// Operators

Game& Game::operator=(const Game& other) {
  ::aithena::Game<State>::operator=(other);

  // InitializeMagic();

  return *this;
}

// Static attributes

const std::array<Figure, 6> Game::figures{
  Figure::kKing, Figure::kQueen, Figure::kRook,
  Figure::kKnight, Figure::kBishop, Figure::kPawn
};

const std::array<Player, 2> Game::players{Player::kWhite, Player::kBlack};

// Chess methods

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

bool Game::IsTerminalState(State& state) {
  // Check for a draw
  if (state.GetNoProgressCount() > max_no_progress_)
    return true;

  if (state.GetMoveCount() > max_move_count_)
    return true;

  // Check for checkmate
  if (GetLegalActions(state).size() == 0)
    return true;

  return false;
}

int Game::GetStateResult(State& state) {
  assert(IsTerminalState(state));

  // Check for a draw
  if (state.GetNoProgressCount() > max_no_progress_
      || state.GetMoveCount() > max_move_count_)
    return 0;

  // Check if we are unable to move
  if (GetLegalActions(state).size() == 0)
    return -1;

  // should never reach here (terminal but can move and not draw)
  assert(false);
}

// Move generation

std::vector<State> Game::GetLegalActions(State state) {
  return GenMoves(state, /*pseudo=*/ true);
}

std::vector<State> Game::GenPawnMoves(State state, unsigned x, unsigned y) {
  std::size_t width{this->GetOption("board_width")};
  std::size_t height{this->GetOption("board_height")};

  Board board_before{state.GetBoard()};

  std::vector<State> moves{};  // Return value

  int direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  BoardPlane figure_plane = board_before.GetCompletePlane();

  signed tmp_dobule_push_pawn_x = state.GetDPushPawnX();
  signed tmp_dobule_push_pawn_y = state.GetDPushPawnY();
  state.SetDPushPawnX(-1);
  state.SetDPushPawnY(-1);

  // Generate pushes
  if (!figure_plane.get(x, y + direction)) {
    if (y + direction == height - 1 || y + direction == 0) {
      // Generate promotions
      for (auto figure : Game::figures) {
        if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;

        moves.push_back(state);
        moves.back().GetBoard().SetField(x, y + direction,
                                         make_piece(figure, state.GetPlayer()));
      }
    } else {
      // Move forward once
      moves.push_back(state);
      moves.back().GetBoard().MoveField(x, y, x, y + direction);

      // Move forward twice
      if (y == (state.GetPlayer() == Player::kWhite ? 1 : height - 2)
          && !figure_plane.get(x, y + 2 * direction)) {
        moves.push_back(state);
        moves.back().GetBoard().MoveField(x, y, x, y + direction * 2);
        moves.back().SetDPushPawnX(static_cast<signed>(x));
        moves.back().SetDPushPawnY(static_cast<signed>(y + direction));
      }
    }
  }

  // Generate captures
  auto opponent = state.GetPlayer() == Player::kWhite
                  ? Player::kBlack : Player::kWhite;
  BoardPlane enemy_figures = board_before.GetPlayerPlane(
                               static_cast<unsigned>(opponent));

  if (x + 1 < width && enemy_figures.get(x + 1, y + direction)) {
    if (y + direction == height - 1 || y + direction == 0) {
      // Generate promotions
      for (auto figure : Game::figures) {
        if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;
        moves.push_back(state);
        moves.back().GetBoard().ClearField(x + 1, y + direction);
        moves.back().GetBoard().SetField(x + 1, y + direction,
                                         make_piece(figure, state.GetPlayer()));
      }
    } else {
      moves.push_back(state);
      moves.back().GetBoard().MoveField(x, y, x + 1, y + direction);
    }
  }

  if (x >= 1 && enemy_figures.get(x - 1, y + direction)) {
    if (y + direction == height - 1 || y + direction == 0) {
      // Generate promotions
      for (auto figure : Game::figures) {
        if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;
        moves.push_back(state);
        moves.back().GetBoard().ClearField(x - 1, y + direction);
        moves.back().GetBoard().SetField(x - 1, y + direction,
                                         make_piece(figure, state.GetPlayer()));
      }
    } else {
      moves.push_back(state);
      moves.back().GetBoard().MoveField(x, y, x - 1, y + direction);
    }
  }

  // en passant
  if (static_cast<unsigned>(tmp_dobule_push_pawn_y) == y + direction) {
    if (static_cast<unsigned>(tmp_dobule_push_pawn_x) == x - 1) {
      State new_state = State{state};
      new_state.GetBoard().MoveField(x, y, x - 1, y + direction);
      new_state.GetBoard().ClearField(x - 1, y);
      moves.push_back(new_state);
    } else if (static_cast<unsigned>(tmp_dobule_push_pawn_x) == x + 1) {
      State new_state = State{state};
      new_state.GetBoard().MoveField(x, y, x + 1, y + direction);
      new_state.GetBoard().ClearField(x + 1, y);
      moves.push_back(new_state);
    }
  }

  // ANy pawn move resets no progress counter
  std::for_each(
    moves.begin(),
    moves.end(),
  [&](State & s) { s.ResetNoProgressCount(); s.GetBoard().ClearField(x, y); });

  state.SetDPushPawnX(tmp_dobule_push_pawn_x);
  state.SetDPushPawnY(tmp_dobule_push_pawn_y);

  return moves;
}

std::vector<State> Game::GenRookMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x,
         y, {up, down, left, right}, 8);
}

std::vector<State> Game::GenBishopMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up + left, up + right, down + left, down + right}, 8);
}

std::vector<State> Game::GenKingMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up, right, down, left, up + right, up + left, down + right, down + left},
  1);
}

std::vector<State> Game::GenQueenMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y,
  {up + left, up + right, down + left, down + right, up, down, left, right}, 8);
}

std::vector<State> Game::GenKnightMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(state, x, y, {
    up + up + left, up + up + right,
    down + down + left, down + down + right,
    up + left + left, up + right + right,
    down + left + left, down + right + right
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

  state.IncNoProgressCount();

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
  [&next_player, &state](State & s) {
    s.IncMoveCount();
    s.SetPlayer(next_player);

    // Any capture resets no progress counter
    if (s.GetBoard().GetFigureCount() < state.GetBoard().GetFigureCount())
      s.ResetNoProgressCount();
  });

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

}  // namespace chess
}  // namespace aithena
