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

bool Game::KingInCheck(State* state, Player player) {
  unsigned width = GetOption("board_width");
  Board& board = state->GetBoard();
  unsigned king_pos =
    board.GetPlayerFigurePlane(player, Figure::kKing).find_first();

  return GeneratePositionAttackers(state, king_pos & width, king_pos / width,
    nullptr, nullptr) > 0;
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

bool Game::SearchForLineAttack(State* state, unsigned x, unsigned y,
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

unsigned Game::SearchForKnightAttack(State* state, unsigned x, unsigned y,
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

unsigned Game::SearchForPawnAttack(State* state, unsigned x, unsigned y,
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

bool Game::SearchForKingAttack(State* state, unsigned x, unsigned y) {
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

unsigned Game::GeneratePositionAttackers(State* state, unsigned x, unsigned y,
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
    attacker_count += SearchForLineAttack(state, x, y, &enemy_rook_queen,
      rook_upper_bb, rook_upper_bb + 2, false, constrained_to, pinned);
  }

  // Get rook-like attackers attacking from lower part
  for (unsigned rook_lower_bb = 0; rook_lower_bb < 2; ++rook_lower_bb) {
    attacker_count += SearchForLineAttack(state, x, y, &enemy_rook_queen,
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
    attacker_count += SearchForLineAttack(state, x, y, &enemy_bishop_queen,
      bishop_upper_bb + 4, bishop_upper_bb + 6, false, constrained_to, pinned);
  }

  // Get bishop-like attackers attacking from lower part
  for (unsigned bishop_lower_bb = 0; bishop_lower_bb < 2; ++bishop_lower_bb) {
    attacker_count += SearchForLineAttack(state, x, y, &enemy_bishop_queen,
      bishop_lower_bb + 6, bishop_lower_bb + 4, true, constrained_to, pinned);
  }

  attacker_count += SearchForKnightAttack(state, x, y, constrained_to);
  attacker_count += SearchForPawnAttack(state, x, y, constrained_to);
  attacker_count += SearchForKingAttack(state, x, y);
  return attacker_count;
}

void Game::GenerateMoveFromBB(State* state, unsigned x,
unsigned y, std::vector<State>* moves, unsigned bb_offset, unsigned inv_bb,
bool lower, bool rook, const BoardPlane& constrained_to,
const BoardPlane& pinned) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");

  Board& current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  Player enemy = state->GetPlayer() == Player::kWhite
      ? Player::kBlack
      : Player::kWhite;

  unsigned arr_pos = y * width + x;

  BoardPlane possible_moves{width, height};
  bool can_attack = false;
  BoardPlane player_figures = current_board.GetPlayerPlane(current_player);
  BoardPlane enemy_figures = current_board.GetPlayerPlane(enemy);
  BoardPlane move_bb =
    magic_bit_planes_[arr_pos * bb_per_pos + bb_offset];

  if (!constrained_to.empty())
    move_bb &= constrained_to;

  if (pinned.get(x, y))
    move_bb &= pinned;

  signed first_enemy_in_path = lower
    ? (move_bb & enemy_figures).msb()
    : (move_bb & enemy_figures).find_first();

  BoardPlane allies_in_path = move_bb & player_figures;
  signed first_ally_in_path = lower ?
    allies_in_path.msb() : allies_in_path.find_first();


  if (first_ally_in_path < 0) {
    // no ally on path
    if (first_enemy_in_path < 0) {
      // no enemy on path
      possible_moves |= move_bb;
    } else {
      // enemy on path
      possible_moves |=
        move_bb & magic_bit_planes_[first_enemy_in_path * bb_per_pos + inv_bb];
      can_attack = true;
    }
  } else {
    // ally on path
    if (first_enemy_in_path < 0) {
      // no enemy on path
      possible_moves |=
        move_bb & magic_bit_planes_[first_ally_in_path * bb_per_pos + inv_bb];
    } else {
      if (lower ? first_ally_in_path > first_enemy_in_path
      : first_ally_in_path < first_enemy_in_path) {
        // ally comes first
        possible_moves |=
          move_bb & magic_bit_planes_[first_ally_in_path * bb_per_pos + inv_bb];
      } else {
        possible_moves |= move_bb
          & magic_bit_planes_[first_enemy_in_path * bb_per_pos + inv_bb];
        can_attack = true;
      }
    }
  }

  signed viable_move = possible_moves.find_first();

  while (viable_move >= 0) {
    moves->push_back(*state);
    moves->back().GetBoard().MoveField(x, y, viable_move % width,
      viable_move / width);
    moves->back().SetDPushPawnX(-1);
    if (rook) {
      if (x == 0)
        moves->back().SetCastleQueen(current_player);
      else if (x == width - 1)
        moves->back().SetCastleKing(current_player);
    }
    viable_move = possible_moves.next(viable_move);
  }

  if (can_attack) {
    moves->push_back(*state);
    moves->back().GetBoard().MoveField(x, y, first_enemy_in_path % width,
      first_enemy_in_path / width);
    moves->back().SetDPushPawnX(-1);
    if (rook) {
      if (x == 0)
        moves->back().SetCastleQueen(current_player);
      else if (x == width - 1)
        moves->back().SetCastleKing(current_player);
    }
    state->ResetNoProgressCount();
  }
}

void Game::GenPawnMoves(State* state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& constrained_to,
const BoardPlane& pinned, bool double_push = true) {
  signed width{this->GetOption("board_width")};
  signed height{this->GetOption("board_height")};

  Board& current_board = state->GetBoard();
  auto opponent = state->GetPlayer() == Player::kWhite
                  ? Player::kBlack : Player::kWhite;

  int direction = state->GetPlayer() == Player::kWhite ? 1 : -1;

  BoardPlane figure_plane = current_board.GetCompletePlane();
  BoardPlane enemy_figures = current_board.GetPlayerPlane(
    static_cast<unsigned>(opponent));
  BoardPlane moveable_fields{width, height};

  signed tmp_dobule_push_pawn_x = state->GetDPushPawnX();
  state->SetDPushPawnX(-1);

  // Generate pushes
  if (!figure_plane.get(x, y + direction)) {
    if (constrained_to.empty() || constrained_to.get(x, y + direction) &&
    (!pinned.get(x, y) || pinned.get(x, y + direction))) {
      // Generate single push
        moveable_fields.set(x, y + direction);
    }
    if (y + 2 * direction >= 0 && y + 2 * direction < height
    && (y == 1 || y == height - 2)
    && !figure_plane.get(x, y + 2 * direction)
    // King is attacked by one piece and attack can be blocked
    && (constrained_to.empty() || constrained_to.get(x, y + 2 * direction))
    // Piece is pinned
    && (!pinned.get(x, y) || pinned.get(x, y + 2 * direction))) {
      // Generate double push
      moveable_fields.set(x, y + 2 * direction);
    }
  }

  // Generate captures
  if (x + 1 < width && enemy_figures.get(x + 1, y + direction)) {
    // enemy on the right side is in capture range
    if (constrained_to.empty() || constrained_to.get(x + 1, y + direction) &&
    (!pinned.get(x, y) || pinned.get(x + 1, y + direction))) {
        // enemy is capturable without endangering own king
        moveable_fields.set(x + 1, y + direction);
    }
  }

  if (x - 1 < width && enemy_figures.get(x - 1, y + direction)) {
    // enemy on the right side is in capture range
    if (constrained_to.empty() || constrained_to.get(x - 1, y + direction) &&
    (!pinned.get(x, y) || pinned.get(x - 1, y + direction))) {
        // enemy is capturable without endangering own king
        moveable_fields.set(x - 1, y + direction);
    }
  }

  // en passant
  if ((opponent == Player::kWhite ? 3 : height - 3) == y) {
    if (static_cast<unsigned>(tmp_dobule_push_pawn_x) == x - 1) {
      // enpassant on left side
      State new_state = State{*state};
      new_state.GetBoard().MoveField(x, y, x - 1, y + direction);
      new_state.GetBoard().ClearField(x - 1, y);
      new_state.ResetNoProgressCount();

      // For enpassant the move is checked for legality manually
      if (!KingInCheck(&new_state, state->GetPlayer()))
        moves->push_back(new_state);
    } else if (static_cast<unsigned>(tmp_dobule_push_pawn_x) == x + 1) {
      // enpassant on right side
      State new_state = State{*state};
      new_state.GetBoard().MoveField(x, y, x + 1, y + direction);
      new_state.GetBoard().ClearField(x + 1, y);
      new_state.ResetNoProgressCount();

      // For enpassant the move is checked for legality manually
      if (!KingInCheck(&new_state, state->GetPlayer()))
        moves->push_back(new_state);
    }
  }

  signed movable_field = moveable_fields.find_first();
  while (movable_field >= 0) {
    moves->push_back(*state);
    if (movable_field / width == y + 2 * direction) {
      // Double push
      moves->back().SetDPushPawnX(x);
      moves->back().GetBoard().MoveField(x, y, movable_field % width,
        movable_field / width);
    } else if (movable_field / width == height - 1
    || movable_field / width == 0) {
      // Promotion
      for (Figure figure : Game::figures) {
        if (figure == Figure::kKing || figure == Figure::kPawn) continue;
        moves->back().GetBoard().SetField(movable_field % width,
          movable_field / width, make_piece(figure, state->GetPlayer()));
          moves->back().GetBoard().ClearField(x, y);
        moves->back().ResetNoProgressCount();
        moves->push_back(*state);
      }
      moves->pop_back();
    } else {
      moves->back().GetBoard().MoveField(x, y, movable_field % width,
        movable_field / width);
    }
    moves->back().ResetNoProgressCount();
    movable_field = moveable_fields.next(movable_field);
  }

  state->SetDPushPawnX(tmp_dobule_push_pawn_x);
}

void Game::GenRookMoves(State* state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& constrained_to,
const BoardPlane& pinned) {
  // Generate moves in direction right
  GenerateMoveFromBB(state, x, y, moves, 0, 2, false,
    true, constrained_to, pinned);

  // Generate moves in direction up
  GenerateMoveFromBB(state, x, y, moves, 1, 3, false,
    true, constrained_to, pinned);

  // Generate moves in direction left
  GenerateMoveFromBB(state, x, y, moves, 2, 0, true,
    true, constrained_to, pinned);

  // Generate moves in direction down
  GenerateMoveFromBB(state, x, y, moves, 3, 1, true,
    true, constrained_to, pinned);
}

void Game::GenBishopMoves(State* state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& constrained_to,
const BoardPlane& pinned) {
  // Generate moves in direction upleft
  GenerateMoveFromBB(state, x, y, moves, 4, 6, false,
    false, constrained_to, pinned);

  // Generate moves in direction upright
  GenerateMoveFromBB(state, x, y, moves, 5, 7, false,
    false, constrained_to, pinned);

  // Generate moves in direction downright
  GenerateMoveFromBB(state, x, y, moves, 6, 4, true,
    false, constrained_to, pinned);

  // Generate moves in direction downleft
  GenerateMoveFromBB(state, x, y, moves, 7, 5, true,
    false, constrained_to, pinned);
}

void Game::GenKingMoves(State *state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& attacked_fields) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");
  Board& current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  Player enemy = state->GetPlayer() == Player::kWhite
      ? Player::kBlack
      : Player::kWhite;

  unsigned arr_pos = y * width + x;
  BoardPlane& king_bb = magic_bit_planes_[arr_pos * bb_per_pos + 11];
  BoardPlane potential_moves =
    !(king_bb & current_board.GetPlayerPlane(current_player)& attacked_fields)
      & king_bb;

  signed move_pos = potential_moves.find_first();
  while (move_pos >= 0) {
    BoardPlane pinned{width, height};
    unsigned attackers = GeneratePositionAttackers(state,
      move_pos % width, move_pos / width, nullptr, &pinned);
    if (attackers < 1 & !pinned.get(x, y)) {
      moves->push_back(*state);
      moves->back().GetBoard().MoveField(
        x, y, move_pos % width, move_pos / width);
      moves->back().SetDPushPawnX(-1);
      moves->back().SetCastleKing(current_player);
      moves->back().SetCastleQueen(current_player);
    }
    move_pos = potential_moves.next(move_pos);
  }
}

void Game::GenQueenMoves(State* state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& constrained_to,
const BoardPlane& pinned) {
  // Generate moves in direction right
  GenerateMoveFromBB(state, x, y, moves, 0, 2, false,
    false, constrained_to, pinned);

  // Generate moves in direction up
  GenerateMoveFromBB(state, x, y, moves, 1, 3, false,
    false, constrained_to, pinned);

  // Generate moves in direction left
  GenerateMoveFromBB(state, x, y, moves, 2, 0, true,
    false, constrained_to, pinned);

  // Generate moves in direction down
  GenerateMoveFromBB(state, x, y, moves, 3, 1, true,
    false, constrained_to, pinned);

  // Generate moves in direction upleft
  GenerateMoveFromBB(state, x, y, moves, 4, 6, false,
    false, constrained_to, pinned);

  // Generate moves in direction upright
  GenerateMoveFromBB(state, x, y, moves, 5, 7, false,
    false, constrained_to, pinned);

  // Generate moves in direction downright
  GenerateMoveFromBB(state, x, y, moves, 6, 4, true,
    false, constrained_to, pinned);

  // Generate moves in direction downleft
  GenerateMoveFromBB(state, x, y, moves, 7, 5, true,
    false, constrained_to, pinned);
}

void Game::GenKnightMoves(State* state, unsigned x, unsigned y,
std::vector<State>* moves, const BoardPlane& constrained_to,
const BoardPlane& pinned) {
  GenerateMoveFromBB(state, x, y, moves, 8, 8, true, false, constrained_to,
    pinned);
}

void Game::GenCastling(State* state, std::vector<State>* moves) {
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");
  if (width < 8 || height < 8 || KingInCheck(state, state->GetPlayer()))
    return;
  Board& current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  unsigned c_height = state->GetPlayer() == Player::kWhite
    ? 0 : 7;

  if (state->GetCastleKing(state->GetPlayer())) {
    // King side castling
    if (!(state->GetBoard().GetCompletePlane().get(5, c_height))
    && !(state->GetBoard().GetCompletePlane().get(6, c_height))) {
      unsigned attacks = 0;
      attacks += GeneratePositionAttackers(state, 5, c_height, nullptr);
      attacks += GeneratePositionAttackers(state, 6, c_height, nullptr);
      if (attacks < 1) {
        moves->push_back(*state);
        moves->back().GetBoard().MoveField(4, c_height, 6, c_height);
        moves->back().GetBoard().MoveField(7, c_height, 5, c_height);
        moves->back().SetCastleKing(state->GetPlayer());
        moves->back().SetCastleQueen(state->GetPlayer());
      }
    }
  }
  if (state->GetCastleQueen(state->GetPlayer())) {
    // King side castling
    if (!(state->GetBoard().GetCompletePlane().get(1, c_height))
    && !(state->GetBoard().GetCompletePlane().get(2, c_height))
    && !(state->GetBoard().GetCompletePlane().get(3, c_height))) {
      unsigned attacks = 0;
      attacks += GeneratePositionAttackers(state, 2, c_height, nullptr);
      attacks += GeneratePositionAttackers(state, 3, c_height, nullptr);
      if (attacks < 1) {
        moves->push_back(*state);
        moves->back().GetBoard().MoveField(4, c_height, 2, c_height);
        moves->back().GetBoard().MoveField(0, c_height, 3, c_height);
        moves->back().SetCastleKing(state->GetPlayer());
        moves->back().SetCastleQueen(state->GetPlayer());
      }
    }
  }
}

std::vector<State> Game::GenMoves(State *state, bool pseudo) {
  // Gather attack information
  signed width = GetOption("board_width");
  signed height = GetOption("board_height");
  std::vector<State> moves;
  // Maximal move count per player
  moves.reserve(124);

  Board current_board = state->GetBoard();
  Player current_player = state->GetPlayer();
  Player enemy = state->GetPlayer() == Player::kWhite
      ? Player::kBlack
      : Player::kWhite;

  BoardPlane constrained{width, height};
  BoardPlane pinned{width, height};

  signed king_pos = state->GetBoard().GetPlayerFigurePlane(
    current_player, Figure::kKing).find_first();

  // Detect check and pinned pieces
  unsigned attackers = king_pos >= 0 ? GeneratePositionAttackers(state,
    king_pos % width, king_pos / width, &constrained, &pinned) : 0;

  // Generate King moves
  if (king_pos >= 0)
    GenKingMoves(state, king_pos % width, king_pos / width, &moves,
      constrained);

  if (attackers > 1) {
    std::for_each(
      moves.begin(),
      moves.end(),
    [state, enemy](State & s) {
      s.IncMoveCount();
      s.SetPlayer(enemy);
      s.SetDPushPawnX(-1);
      if (s.GetBoard().GetFigureCount() < state->GetBoard().GetFigureCount())
        s.ResetNoProgressCount();
    });
    return moves;
  }

  // Generate Queen moves
  signed queen_pos = state->GetBoard().GetPlayerFigurePlane(current_player,
    Figure::kQueen).find_first();

  if (queen_pos >= 0)
    GenQueenMoves(state, queen_pos % width, queen_pos / width, &moves,
      constrained, pinned);

  signed next_pos = -1;

  // Generate Rook moves
  BoardPlane rook_plane =
    state->GetBoard().GetPlayerFigurePlane(current_player, Figure::kRook);

  next_pos = rook_plane.find_first();
  while (next_pos >= 0) {
    GenRookMoves(state, next_pos % width, next_pos / width, &moves, constrained,
      pinned);
    next_pos = rook_plane.next(next_pos);
  }

  // Generate Bishop moves
  BoardPlane bishop =
    state->GetBoard().GetPlayerFigurePlane(current_player, Figure::kBishop);

  next_pos = bishop.find_first();
  while (next_pos >= 0) {
    GenBishopMoves(state, next_pos % width, next_pos / width, &moves,
      constrained, pinned);
    next_pos = bishop.next(next_pos);
  }

  // Generate Knight moves
  BoardPlane knight =
    state->GetBoard().GetPlayerFigurePlane(current_player, Figure::kKnight);

  next_pos = knight.find_first();
  while (next_pos >= 0) {
    GenKnightMoves(state, next_pos % width, next_pos / width, &moves,
      constrained, pinned);
    next_pos = knight.next(next_pos);
  }

  // Generate Castling
  GenCastling(state, &moves);

  std::for_each(
    moves.begin(),
    moves.end(),
  [state](State & s) {
    s.SetDPushPawnX(-1);
    if (s.GetBoard().GetFigureCount() < state->GetBoard().GetFigureCount())
      s.ResetNoProgressCount();
  });

  // Generate Pawn moves
  BoardPlane pawn =
    state->GetBoard().GetPlayerFigurePlane(current_player, Figure::kPawn);

  next_pos = pawn.find_first();
  while (next_pos >= 0) {
    GenPawnMoves(state, next_pos % width, next_pos / width, &moves,
      constrained, pinned);
    next_pos = pawn.next(next_pos);
  }

  std::for_each(
    moves.begin(),
    moves.end(),
  [&enemy](State & s) {
    s.IncMoveCount();
    s.SetPlayer(enemy);
  });
  return moves;
}

std::vector<State> Game::GenMoves(State* state, unsigned x, unsigned y,
bool pseudo) {
  unsigned width = GetOption("board_width");
  unsigned height = GetOption("board_height");
  Piece piece = state->GetBoard().GetField(x, y);
  std::vector<State> moves;
  moves.reserve(14);

  BoardPlane constraint = pseudo
    ? BoardPlane{1, 1}
    : BoardPlane{width, height};

  BoardPlane pinned = pseudo
    ? BoardPlane{1, 1}
    : BoardPlane{width, height};
  // Make checks

  if (!pseudo) {
    unsigned king_pos = state->GetBoard().GetPlayerFigurePlane(
        state->GetPlayer(), Figure::kKing).find_first();
    GeneratePositionAttackers(state, king_pos % width, king_pos / width,
      &constraint, &pinned);
  }

  if (piece.player != state->GetPlayer()
      || piece == kEmptyPiece)
    return moves;

  // Generate pseudo-moves

  state->IncNoProgressCount();

  switch (piece.figure) {
  case Figure::kPawn:
    GenPawnMoves(state, x, y, &moves, constraint, pinned);
    break;
  case Figure::kRook:
    GenRookMoves(state, x, y, &moves, constraint, pinned);
    break;
  case Figure::kBishop:
    GenBishopMoves(state, x, y, &moves, constraint, pinned);
    break;
  case Figure::kQueen:
    GenQueenMoves(state, x, y, &moves, constraint, pinned);
    break;
  case Figure::kKnight:
    GenKnightMoves(state, x, y, &moves, constraint, pinned);
    break;
  case Figure::kKing:
    GenKingMoves(state, x, y, &moves, constraint);
    break;
  case kEmptyPiece.figure:
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
  [&next_player](State & s) {
    s.IncMoveCount();
    s.SetPlayer(next_player);
  });

  return moves;
}

}  // namespace chess
}  // namespace aithena
