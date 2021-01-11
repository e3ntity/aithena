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

  InitializeMagic();
}

Game::Game(const Game& other) : ::aithena::Game<State>(other) {
  InitializeMagic();
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

  assert(width <= 8 && height <= 8);
  static const Coordinate directions[] = {
    right, up, left, down,
    up + left, up + right, down + right, down + left
  };

  for (signed y = 0; y < height; ++y) {
    for (signed x = 0; x < width; ++x) {
      // Create directional move/capture bitmap
      for (Coordinate direction : directions) {
        BoardPlane directional_plane{width, height};
        Coordinate pos{x, y};
        pos.x += direction.x;
        pos.y += direction.y;
        while (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height) {
          directional_plane.set(pos.x, pos.y);
          pos.x += direction.x;
          pos.y += direction.y;
        }
        magic_bit_planes_.push_back(directional_plane);
      }

      // Create knight move/capture bitmap
      BoardPlane knight_bitmap{width, height};
      if (x + 1 < width) {
        if (y + 2 < height)
          knight_bitmap.set(x + 1, y + 2);
        if (y - 2 >= 0)
          knight_bitmap.set(x + 1, y - 2);
      }
      if (x + 2 < width) {
        if (y + 1 < height)
          knight_bitmap.set(x + 2, y + 1);
        if (y - 1 >= 0)
          knight_bitmap.set(x + 2, y - 1);
      }
      if (x - 1 < width) {
        if (y + 2 < height)
          knight_bitmap.set(x - 1, y + 2);
        if (y - 2 >= 0)
          knight_bitmap.set(x - 1, y - 2);
      }
      if (x - 2 < width) {
        if (y + 1 < height)
          knight_bitmap.set(x - 2, y + 1);
        if (y - 1 >= 0)
          knight_bitmap.set(x - 2, y - 1);
      }
      magic_bit_planes_.push_back(knight_bitmap);

      // Create pawn capture bitmap(white then black)
      BoardPlane white_pawn{width, height};
      if (y + 1 < height) {
        if (x + 1 < width)
          white_pawn.set(x + 1, y + 1);
        if (x - 1 >= 0)
          white_pawn.set(x - 1, y + 1);
      }
      magic_bit_planes_.push_back(white_pawn);

      BoardPlane black_pawn{width, height};
      if (y - 1 >= 0) {
        if (x + 1 < width)
          black_pawn.set(x + 1, y - 1);
        if (x - 1 >= 0)
          black_pawn.set(x - 1, y - 1);
      }
      magic_bit_planes_.push_back(black_pawn);

      // Kings move plane
      BoardPlane king_plane{width, height};
      for (Coordinate direction : directions) {
        Coordinate pos{x, y};
        pos.x += direction.x;
        pos.y += direction.y;
        if (x + direction.x >= 0 && x + direction.x < width
          && y + direction.y >= 0 && y + direction.y < height) {
          king_plane.set(x + direction.x, y + direction.y);
        }
      }
      magic_bit_planes_.push_back(king_plane);
    }
  }
}

State Game::GetInitialState() {
  State state(GetOption("board_width"), GetOption("board_height"),
              GetOption("figure_count"));

  assert(GetOption("board_width") == 4 && GetOption("board_height") == 4);

  Board& board = state.GetBoard();

  // Queen
  board.SetField(0, 2, make_piece(Figure::kQueen, Player::kWhite));

  // Kings
  board.SetField(2, 1, make_piece(Figure::kKing, Player::kWhite));
  board.SetField(3, 3, make_piece(Figure::kKing, Player::kBlack));

  return state;
}

/*
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
*/

bool Game::KingInCheck(State state, Player player) {
  Board board = state.GetBoard();
  BoardPlane king_plane = board.GetPlane(make_piece(Figure::kKing, player));
  Player opponent = player == Player::kWhite ? Player::kBlack : Player::kWhite;
  std::vector<State> next_states = GenMoves(&state, /*pseudo=*/ true);

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
  return GenMoves(&state, /*pseudo=*/ false);
}

bool Game::SearchForLineAttack(unsigned x, unsigned y, State* state,
BoardPlane* filtered_enemies, unsigned bb_offset, unsigned inv_bb, bool lower,
BoardPlane* attack_path, BoardPlane* pinned = nullptr) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board& current_board = state->GetBoard();
  unsigned current_player = static_cast<unsigned>(state->GetPlayer());
  unsigned enemy = state->GetPlayer() == Player::kWhite
      ? static_cast<unsigned>(Player::kBlack)
      : static_cast<unsigned>(Player::kWhite);

  unsigned arr_pos = y * width + x;

  BoardPlane player_figures = current_board.GetPlayerPlane(current_player);
  BoardPlane enemy_figures = current_board.GetPlayerPlane(enemy);
  BoardPlane line_of_attack =
      magic_bit_planes_[arr_pos * bb_per_pos + bb_offset];

  signed first_enemy_in_path = lower
    ? (line_of_attack & enemy_figures).msb()
    : (line_of_attack & enemy_figures).find_first();

  BoardPlane allies_in_path = line_of_attack & player_figures;
  signed first_ally_in_path = lower ?
    allies_in_path.msb() : allies_in_path.find_first();
  signed second_ally_in_path = lower
    ? allies_in_path.prev(first_ally_in_path)
    : allies_in_path.next(first_ally_in_path);

  if (first_enemy_in_path < 0
    || !filtered_enemies->get(first_enemy_in_path % width,
        first_enemy_in_path / width)) {
      // No enemy lie in path
      return false;
  } else {
    // at least one enemy is in path
    if (first_ally_in_path < 0
      || (lower ? first_ally_in_path < first_enemy_in_path
            : first_ally_in_path > first_enemy_in_path)) {
      // no ally to block the attack
      if (attack_path) {
        *attack_path |= line_of_attack
            & magic_bit_planes_[first_enemy_in_path * bb_per_pos + inv_bb];
        attack_path->set(first_enemy_in_path);
      }
      return true;
    }
    if (second_ally_in_path >= 0 && (lower
          ? second_ally_in_path > first_enemy_in_path
          : second_ally_in_path < first_enemy_in_path))
      // blocked by two allies, no pinned pieces
      return false;
    if (pinned) {
      *pinned |= line_of_attack & magic_bit_planes_[first_enemy_in_path
              * bb_per_pos + inv_bb];
      pinned->set(first_enemy_in_path);
    }
    return false;
  }
}

unsigned Game::SearchForKnightAttack(unsigned x, unsigned y, State* state,
BoardPlane* constrained_to) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board& current_board = state->GetBoard();
  unsigned current_player = static_cast<unsigned>(state->GetPlayer());
  unsigned enemy = state->GetPlayer() == Player::kWhite
      ? static_cast<unsigned>(Player::kBlack)
      : static_cast<unsigned>(Player::kWhite);

  unsigned arr_pos = y * width + x;

  BoardPlane enemy_knights = current_board.GetPlayerFigurePlane(
    enemy, static_cast<unsigned>(Figure::kKnight));
  BoardPlane attack_bb = magic_bit_planes_[arr_pos * bb_per_pos + 8];
  BoardPlane attacking_knights = attack_bb & enemy_knights;
  if (constrained_to)
    *constrained_to |= attacking_knights;
  return attacking_knights.count();
}

unsigned Game::SearchForPawnAttack(unsigned x, unsigned y, State* state,
BoardPlane* constrained_to) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board& current_board = state->GetBoard();
  unsigned enemy = state->GetPlayer() == Player::kWhite
      ? static_cast<unsigned>(Player::kBlack)
      : static_cast<unsigned>(Player::kWhite);
  signed direction = enemy == static_cast<unsigned>(Player::kWhite) ? -1 : 1;

  BoardPlane enemy_pawns = current_board.GetPlayerFigurePlane(
    enemy, static_cast<unsigned>(Figure::kPawn));
  unsigned count = 0;
  if (y + direction < height && y >= 0) {
    if (x + 1 < width && enemy_pawns.get(x + 1, y + direction)) {
      if (constrained_to)
        constrained_to->set(x + 1, y + direction);
      count++;
    }
    if (x - 1 < width && enemy_pawns.get(x - 1, y + direction)) {
      if (constrained_to)
        constrained_to->set(x - 1, y + direction);
      count++;
    }
  }
  return count;
}

bool Game::SearchForKingAttack(unsigned x, unsigned y, State* state) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board& current_board = state->GetBoard();
  unsigned enemy = state->GetPlayer() == Player::kWhite
      ? static_cast<unsigned>(Player::kBlack)
      : static_cast<unsigned>(Player::kWhite);

  BoardPlane enemy_king = current_board.GetPlayerFigurePlane(
    enemy, static_cast<unsigned>(Figure::kKing));
  unsigned arr_pos = y * width + x;

  return !((enemy_king & magic_bit_planes_[arr_pos * bb_per_pos + 11]).empty());
}

unsigned Game::GeneratePositionAttackers(unsigned x, unsigned y, State* state,
BoardPlane* constrained_to, BoardPlane* pinned = nullptr) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board current_board = state->GetBoard();
  unsigned current_player = static_cast<unsigned>(state->GetPlayer());
  unsigned enemy = state->GetPlayer() == Player::kWhite
      ? static_cast<unsigned>(Player::kBlack)
      : static_cast<unsigned>(Player::kWhite);

  BoardPlane player_figures = current_board.GetPlayerPlane(current_player);
  BoardPlane enemy_figures = current_board.GetPlayerPlane(enemy);
  unsigned arr_pos = y * width + x;

  // Gather information about threads to the given position
  unsigned attacker_count = 0;

  // Get position of enemy pieces that can move like a rook
  BoardPlane enemy_rook_queen =
    current_board.GetPlayerFigurePlane(enemy,
        static_cast<unsigned>(Figure::kRook))
    | current_board.GetPlayerFigurePlane(enemy,
        static_cast<unsigned>(Figure::kQueen));

  // Get rook-like attackers attacking from upper part
  for (unsigned rook_upper_bb = 0; rook_upper_bb < 2; ++rook_upper_bb) {
    attacker_count += SearchForLineAttack(x, y, state, &enemy_rook_queen,
      rook_upper_bb, rook_upper_bb + 2, false, constrained_to, pinned);
  }

  // Get rook-like attackers attacking from lower part
  for (unsigned rook_lower_bb = 0; rook_lower_bb < 2; ++rook_lower_bb) {
    attacker_count += SearchForLineAttack(x, y, state, &enemy_rook_queen,
      rook_lower_bb + 2, rook_lower_bb, true, constrained_to, pinned);
  }

  // Get position of enemy pieces that can move like a bishop
  BoardPlane enemy_bishop_queen =
    current_board.GetPlayerFigurePlane(enemy,
        static_cast<unsigned>(Figure::kBishop))
    | current_board.GetPlayerFigurePlane(enemy,
        static_cast<unsigned>(Figure::kQueen));

  // Get bishop-like attackers attacking from upper part
  for (unsigned bishop_upper_bb = 0; bishop_upper_bb < 2; ++bishop_upper_bb) {
    attacker_count += SearchForLineAttack(x, y, state, &enemy_bishop_queen,
      bishop_upper_bb + 4, bishop_upper_bb + 6, false, constrained_to, pinned);
  }

  // Get bishop-like attackers attacking from lower part
  for (unsigned bishop_lower_bb = 0; bishop_lower_bb < 2; ++bishop_lower_bb) {
    attacker_count += SearchForLineAttack(x, y, state, &enemy_bishop_queen,
      bishop_lower_bb + 6, bishop_lower_bb + 4, true, constrained_to, pinned);
  }

  // Check for attacking knights
  attacker_count += SearchForKnightAttack(x, y, state, constrained_to);
  attacker_count += SearchForPawnAttack(x, y, state, constrained_to);
  attacker_count += SearchForKingAttack(x, y, state);
  return attacker_count;
}

std::vector<State> Game::GenPawnMoves(State state, unsigned x, unsigned y) {
signed width{this->GetOption("board_width")};
signed height{this->GetOption("board_height")};

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

std::vector<State> Game::GenKingMoves(State *state, unsigned x, unsigned y) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");
  std::vector<State> moves;

  Board& current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  Player enemy = state->GetPlayer() == Player::kWhite
      ? Player::kBlack
      : Player::kWhite;

  unsigned arr_pos = y * width + x;
  BoardPlane& king_bb = magic_bit_planes_[arr_pos * bb_per_pos + 11];
  BoardPlane potential_moves =
    !(king_bb & current_board.GetPlayerPlane(current_player)) & king_bb;

  signed move_pos = potential_moves.find_first();
  while (move_pos >= 0) {
    BoardPlane pinned{width, height};
    unsigned attackers = GeneratePositionAttackers(
      move_pos % width, move_pos / width, state, nullptr, &pinned);
    if (attackers < 1 & !pinned.get(x, y)) {
      State new_state{*state};
      new_state.GetBoard().MoveField(x, y, move_pos % width, move_pos / width);
      new_state.SetDPushPawnX(-1);
      new_state.SetDPushPawnY(-1);
      new_state.SetCastleKing(current_player);
      new_state.SetCastleQueen(current_player);
      moves.push_back(new_state);
    }
    move_pos = potential_moves.next(move_pos);
  }
  return moves;
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

std::vector<State> Game::GenMoves(State *state, bool pseudo) {
  // Gather attack information
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");
  std::vector<State> moves;

  Board current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  Player enemy = state->GetPlayer() == Player::kWhite
      ? Player::kBlack
      : Player::kWhite;

  BoardPlane constrained{width, height};
  BoardPlane pinned{width, height};

  unsigned king_pos = state->GetBoard().GetPlayerFigurePlane(
    current_player, Figure::kKing).find_first();

  unsigned attackers = GeneratePositionAttackers(
    king_pos % width, king_pos / width, state, &constrained, &pinned);

  return {};
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
    moves = GenKingMoves(&state, x, y);
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
