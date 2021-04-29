/*
Copyright 2020 All rights reserved.
*/

#include "chess/game.h"
#include "chess/state.h"

#include "board/board.h"
#include "chess/moves.h"
#include "chess/util.h"

namespace aithena {
namespace chess {

// Functions

Piece make_piece(Figure figure, Player player) {
  return Piece{static_cast<unsigned>(figure), static_cast<unsigned>(player)};
}

Player GetOpponent(Player player) {
  return player == Player::kWhite ? Player::kBlack : Player::kWhite;
}

// Constructors

Game::Game() : Game{Options{}} {}

Game::Game(Options options) : ::aithena::Game<State>{options} {
  DefaultOption("board_width", 8);
  DefaultOption("board_height", 8);
  DefaultOption("max_no_progress", 30);
  DefaultOption("max_move_count", 80);
  DefaultOption("figure_count", static_cast<unsigned>(Figure::kCount));

  max_no_progress_ = unsigned(GetOption("max_no_progress"));
  max_move_count_ = unsigned(GetOption("max_move_count"));
}

Game::Game(const Game& other) : ::aithena::Game<State>(other) {
  max_no_progress_ = other.max_no_progress_;
  max_move_count_ = other.max_move_count_;
}

// Operators

Game& Game::operator=(const Game& other) {
  ::aithena::Game<State>::operator=(other);

  // InitializeMagic();

  return *this;
}

// Static attributes

const std::array<Figure, 6> Game::figures{Figure::kKing,   Figure::kQueen,
                                          Figure::kRook,   Figure::kKnight,
                                          Figure::kBishop, Figure::kPawn};

const std::array<Figure, 3> Game::slider_figures{Figure::kQueen, Figure::kRook,
                                                 Figure::kBishop};

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

  return *State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

bool Game::KingInCheck(State state, Player player) {
  Board board = state.GetBoard();
  BoardPlane king_plane = board.GetPlane(make_piece(Figure::kKing, player));
  Player opponent = player == Player::kWhite ? Player::kBlack : Player::kWhite;

  state.SetPlayer(opponent);
  std::vector<State> next_states = GenPseudoMoves(state);

  for (auto next_state : next_states) {
    if ((king_plane & next_state.GetBoard().GetPlayerPlane(opponent)).empty())
      continue;
    return true;
  }

  return false;
}

bool Game::IsTerminalState(State& state) {
  // Check for a draw
  if (state.GetNoProgressCount() > unsigned(GetOption("max_no_progress")))
    return true;

  if (state.GetMoveCount() > unsigned(GetOption("max_move_count"))) return true;

  // Check for checkmate
  if (GetLegalActions(state).size() == 0) return true;

  return false;
}

int Game::GetStateResult(State& state) {
  assert(IsTerminalState(state));

  // Check for a draw
  if (state.GetNoProgressCount() > max_no_progress_ ||
      state.GetMoveCount() > max_move_count_)
    return 0;

  // Check if we are unable to move and king is in check
  if (GetLegalActions(state).size() == 0) {
    if (KingInCheck(state, state.GetPlayer())) return -1;

    return 0;
  }

  assert(false);  // Should never reach here.
}

// Move generation

std::vector<State> Game::GetLegalActions(State state) {
  return GenMoves(state);
}

std::vector<State> Game::GenPawnPushes(State state, int x, int y) {
  Board board = state.GetBoard();
  int width = board.GetWidth();
  int height = board.GetHeight();

  std::vector<State> moves;

  int direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  // Single push

  // Out of bounds
  if (y + direction >= height || y + direction < 0) return moves;
  // Blocked
  if (!(board.GetField(x, y + direction) == kEmptyPiece)) return moves;

  if (y + direction == height - 1 || y + direction == 0) {
    // Promotion
    for (auto figure : Game::figures) {
      State promo{state};

      if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;

      promo.GetBoard().ClearField(x, y);
      promo.GetBoard().SetField(x, y + direction,
                                make_piece(figure, state.GetPlayer()));
      promo.SetDPushPawn({-1, -1});
      moves.push_back(promo);
    }
  } else {
    // Normal push
    moves.push_back(state);
    moves.back().GetBoard().MoveField(x, y, x, y + direction);
    moves.back().SetDPushPawn({-1, -1});
  }

  // Double push

  // Out of bounds
  if (y + 2 * direction >= height || y + 2 * direction < 0) return moves;
  // Not at start position
  if (y != (state.GetPlayer() == Player::kWhite ? 1 : height - 2)) return moves;
  // Blocked
  if (!(board.GetField(x, y + 2 * direction) == kEmptyPiece)) return moves;

  moves.push_back(state);
  moves.back().GetBoard().MoveField(x, y, x, y + 2 * direction);
  moves.back().SetDPushPawn({x, y + direction});

  // Reset no progress count for each move.
  std::for_each(moves.begin(), moves.end(),
                [&](State& s) { s.ResetNoProgressCount(); });

  return moves;
}

std::vector<State> Game::GenRawPawnCaptures(State state, int x, int y) {
  // TODO: this function is copied from GenPawnCaptures. This is bad!
  // Maybe use GenPawnCaptures and add captures that disregard whether there
  // is a piece to capture.
  Board board = state.GetBoard();
  int width = board.GetWidth();
  int height = board.GetHeight();

  std::vector<State> moves;

  int direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  if (y + direction >= height || y + direction < 0) return moves;

  static int sides[2] = {-1, 1};

  for (int h : sides) {
    if (x + h >= width || x + h < 0) continue;

    State move{state};
    Coord move_ep = move.GetDPushPawn();

    Piece captured_piece = board.GetField(x + h, y + direction);

    if (move_ep.x == x + h && move_ep.y == y + direction)
      // En passant, remove captured pawn
      move.GetBoard().ClearField(move_ep.x, move_ep.y - direction);
    else if (captured_piece.player == state.GetPlayer())
      // Blocked
      continue;

    move.SetDPushPawn({-1, -1});  // Any pawn capture clears this field
    move.SetNoProgressCount(0);

    // Normal capture
    if (y + direction > 0 && y + direction < height - 1) {
      move.GetBoard().MoveField(x, y, x + h, y + direction);
      moves.push_back(move);

      continue;
    }

    // Promotion
    for (auto figure : Game::figures) {
      State promo{move};

      if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;

      promo.GetBoard().ClearField(x, y);
      promo.GetBoard().SetField(x + h, y + direction,
                                make_piece(figure, state.GetPlayer()));
      promo.SetDPushPawn({-1, -1});
      moves.push_back(promo);
    }
  }

  return moves;
}

std::vector<State> Game::GenPawnCaptures(State state, int x, int y) {
  Board board = state.GetBoard();
  int width = board.GetWidth();
  int height = board.GetHeight();

  std::vector<State> moves;

  int direction = state.GetPlayer() == Player::kWhite ? 1 : -1;

  if (y + direction >= height || y + direction < 0) return moves;

  static int sides[2] = {-1, 1};

  for (int h : sides) {
    if (x + h >= width || x + h < 0) continue;

    State move{state};
    Coord move_ep = move.GetDPushPawn();

    Piece captured_piece = board.GetField(x + h, y + direction);

    if (move_ep.x == x + h && move_ep.y == y + direction)
      // En passant, remove captured pawn
      move.GetBoard().ClearField(move_ep.x, move_ep.y - direction);
    else if (captured_piece == kEmptyPiece)
      // No piece to capture
      continue;
    else if (captured_piece.player == state.GetPlayer())
      // Blocked
      continue;

    move.SetDPushPawn({-1, -1});  // Any pawn capture clears this field
    move.SetNoProgressCount(0);

    // Normal capture
    if (y + direction > 0 && y + direction < height - 1) {
      move.GetBoard().MoveField(x, y, x + h, y + direction);
      moves.push_back(move);

      continue;
    }

    // Promotion
    for (auto figure : Game::figures) {
      State promo{move};

      if ((figure == Figure::kPawn) || figure == Figure::kKing) continue;

      promo.GetBoard().ClearField(x, y);
      promo.GetBoard().SetField(x + h, y + direction,
                                make_piece(figure, state.GetPlayer()));
      promo.SetDPushPawn({-1, -1});
      moves.push_back(promo);
    }
  }

  return moves;
}

std::vector<State> Game::GenPawnMoves(State state, unsigned x, unsigned y) {
  std::vector<State> moves = GenPawnPushes(state, x, y);

  std::vector<State> captures = GenPawnCaptures(state, x, y);
  moves.insert(moves.end(), captures.begin(), captures.end());

  return moves;
}

std::vector<State> Game::GenRookMoves(State state, unsigned x, unsigned y) {
  std::vector<State> moves = aithena::chess::GenDirectionalMoves(
      state, x, y, {up, down, left, right}, 8);

  int width = state.GetBoard().GetWidth();
  Player player = state.GetPlayer();

  if (x == 0 && state.GetCastleQueen(player)) {
    std::for_each(moves.begin(), moves.end(), [&player, &state](State& s) {
      state.SetCastleQueen(player);
    });
  } else if (x == width - 1 && state.GetCastleKing(player)) {
    std::for_each(moves.begin(), moves.end(),
                  [&player, &state](State& s) { state.SetCastleKing(player); });
  }

  return moves;
}

std::vector<State> Game::GenBishopMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(
      state, x, y, {up + left, up + right, down + left, down + right}, 8);
}

std::vector<State> Game::GenCastlingMoves(State state, int x, int y) {
  std::vector<State> moves;

  Board& board = state.GetBoard();

  int height = board.GetHeight();
  int width = board.GetWidth();

  if (width < 6 || width > 8) return moves;
  if (y > 0 && y < height - 1) return moves;

  int x_rook_left = -1;
  int x_rook_right = -1;

  for (int i = 0; i < width; ++i) {
    Piece p = board.GetField(i, y);

    if (p.figure != static_cast<int>(Figure::kRook) ||
        p.player != state.GetPlayer())
      continue;

    // Take rooks closest to king
    if (i < x) {
      x_rook_left = i;
    } else {
      x_rook_right = i;
      break;
    }
  }

  BoardPlane player_plane = board.GetPlayerPlane(state.GetPlayer());

  if (x_rook_right >= 0 && state.GetCastleKing(state.GetPlayer())) {
    BoardPlane occupied_king(width, height);

    occupied_king.ScanLine(x + 1, y, x_rook_right, y);
    occupied_king &= player_plane;

    if (occupied_king.count() == 0) {
      moves.push_back(state);
      moves.back().GetBoard().MoveField(x_rook_right, y, x + 1, y);
      moves.back().GetBoard().MoveField(x, y, x + 2, y);
      moves.back().SetCastleKing(state.GetPlayer());
      moves.back().SetCastleQueen(state.GetPlayer());
    }
  }

  if (x_rook_left >= 0 && state.GetCastleQueen(state.GetPlayer())) {
    BoardPlane occupied_queen(width, height);

    occupied_queen.ScanLine(x - 1, y, x_rook_left, y);
    occupied_queen &= player_plane;

    if (occupied_queen.count() == 0) {
      moves.push_back(state);
      moves.back().GetBoard().MoveField(x_rook_left, y, x - 1, y);
      moves.back().GetBoard().MoveField(x, y, x - 2, y);
      moves.back().SetCastleKing(state.GetPlayer());
      moves.back().SetCastleQueen(state.GetPlayer());
    }
  }

  return moves;
}

std::vector<State> Game::GenKingMoves(State state, unsigned x, unsigned y) {
  std::vector<State> moves = aithena::chess::GenDirectionalMoves(
      state, x, y,
      {up, right, down, left, up + right, up + left, down + right, down + left},
      1);

  // Any king move disables futher castling
  std::for_each(moves.begin(), moves.end(), [&state](State& s) {
    s.SetCastleKing(state.GetPlayer());
    s.SetCastleQueen(state.GetPlayer());
  });

  return moves;
}

std::vector<State> Game::GenQueenMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(
      state, x, y,
      {up + left, up + right, down + left, down + right, up, down, left, right},
      8);
}

std::vector<State> Game::GenKnightMoves(State state, unsigned x, unsigned y) {
  return aithena::chess::GenDirectionalMoves(
      state, x, y,
      {up + up + left, up + up + right, down + down + left, down + down + right,
       up + left + left, up + right + right, down + left + left,
       down + right + right},
      1);
}

std::vector<State> Game::GenPseudoMoves(State state) {
  Board board = state.GetBoard();
  unsigned width = board.GetWidth();
  unsigned height = board.GetHeight();

  std::vector<State> moves;

  for (unsigned y = 0; y < height; ++y) {
    for (unsigned x = 0; x < width; ++x) {
      std::vector<State> piece_moves = GenPseudoMoves(state, x, y);
      moves.reserve(moves.size() + piece_moves.size());
      moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
    }
  }

  return moves;
}

std::vector<State> Game::PreparePseudoMoves(State state,
                                            std::vector<State> moves) {
  Player next_player = state.GetOpponent();

  std::for_each(moves.begin(), moves.end(), [&next_player, &state](State& s) {
    s.IncMoveCount();
    s.SetPlayer(next_player);

    // Any capture resets no progress counter
    if (s.GetBoard().GetFigureCount() < state.GetBoard().GetFigureCount())
      s.ResetNoProgressCount();
    else
      s.IncNoProgressCount();
  });

  return moves;
}

std::vector<State> Game::GenPseudoMoves(State state, unsigned x, unsigned y) {
  Piece piece = state.GetBoard().GetField(x, y);
  std::vector<State> moves;

  // Make checks

  if (piece.player != static_cast<unsigned>(state.GetPlayer()) ||
      piece == kEmptyPiece)
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
      break;
    case kEmptyPiece.figure:
      break;
    default:
      assert(false);
  }

  return PreparePseudoMoves(state, moves);
}

BoardPlane Game::GetAttackers(State state, int x, int y) {
  Board& board = state.GetBoard();
  State attack_state{state};
  Board& attack_board = attack_state.GetBoard();
  BoardPlane attacks(board.GetWidth(), board.GetHeight());

  for (auto fig : figures) {
    attack_board.SetField(x, y, make_piece(fig, state.GetPlayer()));

    auto attack_moves = GenPseudoMoves(attack_state, x, y);
    BoardPlane attackers = board.GetPlane(make_piece(fig, state.GetOpponent()));

    for (auto move : attack_moves)
      attacks |= (GetNewFields(attack_board, move.GetBoard()) & attackers);
  }

  return attacks;
}

std::vector<std::tuple<Coord, Coord>> Game::GetPins(State state, int x, int y) {
  // Find all pieces that 1) belong to player and can be atacked by an
  // opponent sliding piece, 2) lie on a straight line between an opponent
  // sliding piece and the piece at (x, y) and 3) can be moved to by queen
  // moves from the piece at (x, y).

  Board& board = state.GetBoard();
  // Bitboard with pinned pieces (to be created & returned)
  std::vector<std::tuple<Coord, Coord>> pins;
  // Piece at (x, y)
  Piece piece = board.GetField(x, y);

  if (piece == kEmptyPiece) return pins;

  Player player = static_cast<Player>(piece.player);
  Player opponent = GetOpponent(player);
  BoardPlane player_plane = board.GetPlayerPlane(player);

  state.SetPlayer(opponent);

  // Generate queen moves for piece at (x, y).
  BoardPlane queen_move_mask(board.GetWidth(), board.GetHeight());
  board.SetField(x, y, make_piece(Figure::kQueen, opponent));

  for (auto move : GenQueenMoves(state, x, y))
    queen_move_mask |= GetNewFields(board, move.GetBoard());

  board.SetField(x, y, piece);

  // Find all fields that an opponent piece can move to and that are on a
  // a straight line towards piece at (x, y).
  for (auto figure : slider_figures) {
    auto coords = board.GetPlane(make_piece(figure, opponent)).GetCoords();

    for (auto coord : coords) {
      BoardPlane pin_mask(board.GetWidth(), board.GetHeight());
      auto attack_moves = GenPseudoMoves(state, coord.x, coord.y);

      for (auto move : attack_moves)
        pin_mask |= GetNewFields(board, move.GetBoard());

      BoardPlane line(board.GetWidth(), board.GetHeight());
      line.ScanLine(x, y, coord.x, coord.y);
      line.clear(x, y);

      // Filter out moves that are not on a straight line between the two
      // attacker and piece at (x, y)
      pin_mask &= line;
      // Filter out fields that are not within queen-move range of piece at
      // (x, y)
      pin_mask &= queen_move_mask;
      // Filter out fields that don't contain player's pieces
      pin_mask &= player_plane;

      Coords pinned = pin_mask.GetCoords();

      if (pinned.size() == 0) continue;

      assert(pinned.size() == 1);

      pins.push_back(std::make_tuple(coord, pinned.at(0)));
    }
  }

  return pins;
}

bool Game::IsCapture(Board& b1, Board& b2) {
  return b1.GetCompletePlane().count() > b2.GetCompletePlane().count();
}

bool Game::IsEnPassant(Board& b1, Board& b2) {
  BoardPlane before = b1.GetFigurePlane(static_cast<int>(Figure::kPawn));
  BoardPlane after = b2.GetFigurePlane(static_cast<int>(Figure::kPawn));

  // Not a pawn-capture
  if (before.count() <= after.count()) return false;
  // An extra field is involved in en-passant captures
  if ((before | after).count() == before.count()) return false;

  return true;
}

bool Game::IsEnPassantDiscoveredCheck(Board& b1, Board& b2) {
  if (!IsEnPassant(b1, b2)) return false;

  // TODO: implement

  return false;
}

State Game::FlipMostPieces(State state, Player player, Coord place) {
  Board& board = state.GetBoard();

  for (int y = 0; y < board.GetHeight(); ++y) {
    for (int x = 0; x < board.GetWidth(); ++x) {
      if (place.x == x && place.y == y) continue;

      Piece p = board.GetField(x, y);

      if (p == kEmptyPiece || p.player == player) continue;

      board.SetField(x, y, make_piece(static_cast<Figure>(p.figure), player));
    }
  }
  return state;
}

std::vector<State> Game::GenMoves(State state) {
  // Vector of all legal moves (to be returned)
  std::vector<State> moves;

  if (state.GetMoveCount() > GetOption("max_move_count")) return moves;
  if (state.GetNoProgressCount() > GetOption("max_no_progress")) return moves;

  Board& board = state.GetBoard();
  int width = board.GetWidth();
  int height = board.GetHeight();

  Piece player_king = make_piece(Figure::kKing, state.GetPlayer());
  Piece opponent_king = make_piece(Figure::kKing, state.GetOpponent());

  auto king_coords = board.FindPiece(player_king);

  if (king_coords.size() != 1) {
    assert(false);
    return moves;  // if assertions are disabled
  }

  int king_x = king_coords.at(0).x;
  int king_y = king_coords.at(0).y;

  // Remove player's king from board and set all pieces to belong to player.
  // Then, loop through the opponent's pieces and compute all possible attacks
  // to generate a bitboard of all fields, dangerous to the king.
  State king_danger_state(state);
  king_danger_state.GetBoard().SetField(king_x, king_y, kEmptyPiece);
  king_danger_state.SetPlayer(state.GetOpponent());

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Piece p = king_danger_state.GetBoard().GetField(x, y);

      if (p == kEmptyPiece || p.player == static_cast<int>(state.GetPlayer()))
        continue;

      king_danger_state.GetBoard().SetField(
          x, y, make_piece(static_cast<Figure>(p.figure), state.GetPlayer()));
    }
  }

  // Squares that the king should never move to
  BoardPlane king_danger_squares(width, height);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      std::vector<State> nk_moves;

      Piece piece = board.GetField(x, y);

      if (piece.player != state.GetOpponent()) continue;

      // Copy state and set figure at (x, y) to be opponent's for computing
      // possible attacks
      State danger_state(king_danger_state);
      danger_state.GetBoard().SetField(
          x, y,
          make_piece(static_cast<Figure>(piece.figure), state.GetOpponent()));

      if (piece.figure == static_cast<int>(Figure::kPawn)) {
        nk_moves = PreparePseudoMoves(danger_state,
                                      GenRawPawnCaptures(danger_state, x, y));
      } else {
        nk_moves = GenPseudoMoves(danger_state, x, y);
      }

      for (auto move : nk_moves) {
        king_danger_squares |=
            GetNewFields(danger_state.GetBoard(), move.GetBoard());
      }
    }
  }

  // Add all moves the king can make without getting into check
  for (auto move : GenPseudoMoves(state, king_x, king_y)) {
    if (!(move.GetBoard().GetPlane(player_king) & king_danger_squares).empty())
      continue;

    moves.push_back(move);
  }

  // Squares with pieces that attack the king
  BoardPlane king_checks = GetAttackers(state, king_x, king_y);

  // If there is more than one check on the king, only king moves are valid
  if (king_checks.count() > 1) return moves;

  // Capture mask indicates on which squares pieces can be captured,
  // and push mask indicates on which squares pieces can be moved to avoid
  // check
  BoardPlane capture_mask(width, height);
  BoardPlane push_mask(width, height);

  capture_mask |= !capture_mask;
  push_mask |= !push_mask;

  if (king_checks.count() == 1) {
    capture_mask &= king_checks;
    push_mask ^= push_mask;

    auto attack_coords = king_checks.GetCoords().at(0);
    int attack_x = attack_coords.x;
    int attack_y = attack_coords.y;

    Piece attack_piece = board.GetField(attack_x, attack_y);

    switch (static_cast<Figure>(attack_piece.figure)) {
      case Figure::kQueen:
      case Figure::kRook:
      case Figure::kBishop:
        push_mask.ScanLine(attack_x, attack_y, king_x, king_y);
        push_mask.clear(attack_x, attack_y);
        push_mask.clear(king_x, king_y);
        break;
      default:
        break;
    }
  }

  // Generate moves for pinned pieces and remove them from the board.
  std::vector<std::tuple<Coord, Coord>> pins = GetPins(state, king_x, king_y);
  BoardPlane pin_mask(width, height);

  for (auto pin : pins) {
    BoardPlane pin_move_mask(width, height);
    auto pinner = std::get<0>(pin);
    auto pinned = std::get<1>(pin);

    pin_move_mask.ScanLine(pinner.x, pinner.y, pinned.x, pinned.y);
    pin_move_mask.clear(pinned.x, pinned.y);

    auto pin_moves = GenPseudoMoves(state, pinned.x, pinned.y);

    for (auto move : pin_moves) {
      if ((GetNewFields(board, move.GetBoard()) & pin_move_mask).count() == 0)
        continue;
      moves.push_back(move);
    }

    pin_mask.set(pinned.x, pinned.y);
  }

  /*
  std::cout << "king danger squares" << std::endl;
  std::cout << PrintMarkedBoard(state, king_danger_squares) << std::endl;
  std::cout << "king checks" << std::endl;
  std::cout << PrintMarkedBoard(state, king_checks) << std::endl;
  std::cout << "pin mask" << std::endl;
  std::cout << PrintMarkedBoard(state, pin_mask) << std::endl;
  std::cout << "capture mask" << std::endl;
  std::cout << PrintMarkedBoard(state, capture_mask) << std::endl;
  std::cout << "push mask" << std::endl;
  std::cout << PrintMarkedBoard(state, push_mask) << std::endl;
  */

  // Generate all other moves
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      std::vector<State> pseudo_moves;
      // Skip pinned pieces
      if (pin_mask.get(x, y) == 1) continue;

      if (x == king_x && y == king_y) {
        if (king_checks.count() > 0) continue;

        auto king_pseudo_moves = GenCastlingMoves(state, x, y);

        // Remove castle through check
        for (auto move : king_pseudo_moves) {
          auto passed_fields = GetNewFields(board, move.GetBoard());

          if (!((passed_fields & king_danger_squares).empty())) continue;

          pseudo_moves.push_back(move);
        }
      } else {
        pseudo_moves = GenPseudoMoves(state, x, y);
      }

      for (auto move : pseudo_moves) {
        BoardPlane target_field(width, height);

        if (IsCapture(board, move.GetBoard()))
          // Fields where a figure was removed from
          target_field = GetNewFields(move.GetBoard(), board) & capture_mask;
        else
          // Fields where a figure was moved to
          target_field = GetNewFields(board, move.GetBoard()) & push_mask;

        // Not a valid move
        if (target_field.count() == 0) continue;

        // Filter out en-passant discovered checks
        if (IsEnPassantDiscoveredCheck(board, move.GetBoard())) continue;

        moves.push_back(move);
      }
    }
  }

  return moves;
}

}  // namespace chess
}  // namespace aithena
