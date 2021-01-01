/*
Copyright 2020 All rights reserved.
*/
#include <algorithm>
#include <iostream>

#include "gtest/gtest.h"

#include "board/board.h"
#include "chess/game.h"

/**
 * capital letters are white figures and small letters are black figures
 * X - King
 * Q - Queen
 * R - Rook
 * B - Bishop
 * K - Knight
 * P - Pawn
 */

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker) {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;

  std::string turn = state.GetPlayer() == aithena::chess::Player::kWhite
                     ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1)
                            ? "\033[1;47m"
                            : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white = piece.player ==
              static_cast<unsigned>(aithena::chess::Player::kWhite);
      std::string s_piece;

      // Figure icon
      switch (piece.figure) {
      case static_cast<unsigned>(aithena::chess::Figure::kKing):
        s_piece = white ? "♔" : "♚";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kQueen):
        s_piece = white ? "♕" : "♛";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kRook):
        s_piece = white ? "♖" : "♜";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kBishop):
        s_piece = white ? "♗" : "♝";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kKnight):
        s_piece = white ? "♘" : "♞";
        break;
      case static_cast<unsigned>(aithena::chess::Figure::kPawn):
        s_piece = white ? "♙" : "♟︎";
        break;
      default:
        s_piece += std::to_string(piece.figure);
        break;
      }

      bool marked = marker.get(x, y);

      if (piece == aithena::kEmptyPiece) {
        repr << s_color << (marked ? "\033[31m- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? "\033[31m-" : " ") << s_color << s_piece
             << "\033[24m" << (marked ? "\033[31m-" : " ") << "\033[0m";
      }
    }
  }
  repr << std::endl << "   A  B  C  D  E  F  G  H" << std::endl;

  return repr.str();
}

std::string PrintBoard(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

/**
 * Generates a Move in manual fashion
 */
aithena::chess::State GenMoveManually(aithena::chess::State fromState,
    unsigned x_start, unsigned y_start, unsigned x_dest, unsigned y_dest,
    bool reset_no_progress = true) {

  aithena::chess::State newState{fromState};
  newState.GetBoard().MoveField(x_start, y_start, x_dest, y_dest);
  newState.IncMoveCount();
  if (reset_no_progress)
    newState.ResetNoProgressCount();
  newState.SetPlayer(
    newState.GetPlayer() == aithena::chess::Player::kBlack
      ? aithena::chess::Player::kWhite
      : aithena::chess::Player::kBlack);
  newState.SetDPushPawnX(-1);
  newState.SetDPushPawnY(-1);
  return newState;
}

/**
 * Generates a Capture in manual fashion
 */
aithena::chess::State GenCaptureManually(aithena::chess::State fromState,
    unsigned x_start, unsigned y_start, unsigned x_dest, unsigned y_dest) {

  aithena::chess::State newState{fromState};
  newState.GetBoard().MoveField(x_start, y_start, x_dest, y_dest);
  newState.IncMoveCount();
  newState.ResetNoProgressCount();
  newState.SetPlayer(
    newState.GetPlayer() == aithena::chess::Player::kBlack
      ? aithena::chess::Player::kWhite
      : aithena::chess::Player::kBlack);
  newState.SetDPushPawnX(-1);
  newState.SetDPushPawnY(-1);
  return newState;
}

/**
 * Test for
 *   A B C D
 * 1 r q x r
 * 2 - - - -
 * 3 - - - -
 * 4 R Q X R
 * 
 * Test for generic initial state for a 4x4 chess
 * White's turn
 */
TEST(MoveGenerationTest, default_state_moves) {
  aithena::chess::Game::Options options {
    {"board_width", 4},
    {"board_height", 4}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(4),
    static_cast<std::size_t>(4),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
    board.SetField(0, 0, aithena::chess::make_piece(
          aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));
    board.SetField(1, 0, aithena::chess::make_piece(
          aithena::chess::Figure::kQueen, aithena::chess::Player::kWhite));
    board.SetField(2, 0, aithena::chess::make_piece(
          aithena::chess::Figure::kKing, aithena::chess::Player::kWhite));
    board.SetField(3, 0, aithena::chess::make_piece(
          aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));
    board.SetField(0, 3, aithena::chess::make_piece(
          aithena::chess::Figure::kRook, aithena::chess::Player::kBlack));
    board.SetField(1, 3, aithena::chess::make_piece(
          aithena::chess::Figure::kQueen, aithena::chess::Player::kBlack));
    board.SetField(2, 3, aithena::chess::make_piece(
          aithena::chess::Figure::kKing, aithena::chess::Player::kBlack));
    board.SetField(3, 3, aithena::chess::make_piece(
          aithena::chess::Figure::kRook, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 13);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 0, 0, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 0, 0, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenCaptureManually(initstate, 0, 0, 0, 3)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 1, 0, 0, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 1, 0, 1, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 1, 0, 1, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenCaptureManually(initstate, 1, 0, 1, 3)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 1, 0, 2, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 1, 0, 3, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 2, 0, 2, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 3, 0, 3, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 3, 0, 3, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenCaptureManually(initstate, 3, 0, 3, 3)) != moves_generated.end());
}

/**
 * Test for
 *   A B C
 * 1 - - X
 * 2 - - -
 * 3 - k -
 * 4 - - -
 * 5 - Q -
 * 
 * White King is in check. Player white can either move the king or capture the
 * the black knight with the queen.
 * White's turn
 */
TEST(MoveGenerationTest, KingInCheckTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 3},
    {"board_height", 5}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(3),
    static_cast<std::size_t>(5),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(2, 4, aithena::chess::make_piece(
        aithena::chess::Figure::kKing, aithena::chess::Player::kWhite));
  board.SetField(1, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kWhite));
  board.SetField(1, 2, aithena::chess::make_piece(
        aithena::chess::Figure::kKnight, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 4);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenCaptureManually(initstate, 1, 0, 1, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 2, 4, 1, 4)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 2, 4, 1, 3)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 2, 4, 2, 3)) != moves_generated.end());
}

/**
 * Test for
 *   A B
 * 1 - -
 * 2 - -
 * 3 p -
 * 4 - -
 * 5 - P
 * 6 - -
 * 
 * Enpassant test
 * White's turn
 */
TEST(MoveGenerationTest, EnPassantTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 2},
    {"board_height", 6}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(2),
    static_cast<std::size_t>(5),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 3, aithena::chess::make_piece(
        aithena::chess::Figure::kPawn, aithena::chess::Player::kBlack));
  board.SetField(1, 1, aithena::chess::make_piece(
        aithena::chess::Figure::kPawn, aithena::chess::Player::kWhite));

  auto double_push_state = game.GenMoves(initstate)[1];
  auto moves_generated = game.GenMoves(double_push_state);

  ASSERT_EQ(moves_generated.size(), 2);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(double_push_state, 0, 3, 0, 2)) != moves_generated.end());
  auto enpassant = GenMoveManually(double_push_state, 0, 3, 1, 2);
  enpassant.GetBoard().ClearField(1, 3);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      enpassant) != moves_generated.end());
}

/**
 * Test for
 *   A B C D
 * 1 X - - -
 * 2 - - q -
 * 3 - k k -
 * 4 - - - -
 * 
 * Check mate for player white
 * White's turn
 */
TEST(MoveGenerationTest, CheckMateTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 4},
    {"board_height", 4}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(4),
    static_cast<std::size_t>(4),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 3, aithena::chess::make_piece(
        aithena::chess::Figure::kKing, aithena::chess::Player::kWhite));
  board.SetField(1, 1, aithena::chess::make_piece(
        aithena::chess::Figure::kKnight, aithena::chess::Player::kBlack));
  board.SetField(2, 1, aithena::chess::make_piece(
        aithena::chess::Figure::kKnight, aithena::chess::Player::kBlack));
  board.SetField(2, 2, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 0);
}

/**
 * Test for
 *   A B C D E F
 * 1 X - - - - -
 * 2 Q - - - r -
 * 3 - - - - - -
 * 4 - - - - - -
 * 5 - r - - - -
 * 6 q - - - - -
 * 
 * White Queen is pinned along A
 * White's turn
 */
TEST(MoveGenerationTest, PinnedPieceTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 6},
    {"board_height", 6}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(6),
    static_cast<std::size_t>(6),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 5, aithena::chess::make_piece(
        aithena::chess::Figure::kKing, aithena::chess::Player::kWhite));
  board.SetField(0, 4, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kWhite));
  board.SetField(0, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kBlack));
  board.SetField(5, 4, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kBlack));
  board.SetField(1, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 4);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 4, 0, 3)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 4, 0, 2)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 4, 0, 1)) != moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenCaptureManually(initstate, 0, 4, 0, 0)) != moves_generated.end());
}

/**
 * Test for
 *   A B C D E F
 * 1 x - - - - R
 * 2 -  - - - -
 * 3 - - - k - -
 * 4 - - b - - -
 * 5 - - - - - -
 * 6 R - - - - q
 * 
 * Black king is double checked hence only moving the king is possible for
 * player black. Black's turn.
 */
TEST(MoveGenerationTest, DoubleCheckTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 6},
    {"board_height", 6}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(6),
    static_cast<std::size_t>(6),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };
  initstate.SetPlayer(aithena::chess::Player::kBlack);

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 5, aithena::chess::make_piece(
        aithena::chess::Figure::kKing, aithena::chess::Player::kBlack));
  board.SetField(5, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kBlack));
  board.SetField(0, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));
  board.SetField(5, 5, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));
  board.SetField(2, 2, aithena::chess::make_piece(
        aithena::chess::Figure::kBishop, aithena::chess::Player::kBlack));
  board.SetField(3, 3, aithena::chess::make_piece(
        aithena::chess::Figure::kKnight, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 1);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      GenMoveManually(initstate, 0, 5, 1, 4)) != moves_generated.end());
}

/**
 * Test for
 *   A B C
 * 1 r - q
 * 2 - P -
 * 
 * Promotion by move or capture
 * White's turn
 */
TEST(MoveGenerationTest, PromotionTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 3},
    {"board_height", 2}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(3),
    static_cast<std::size_t>(2),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(1, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kPawn, aithena::chess::Player::kWhite));
  board.SetField(0, 1, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kBlack));
  board.SetField(2, 1, aithena::chess::make_piece(
        aithena::chess::Figure::kQueen, aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 12);
  auto promotion_by_push = GenMoveManually(initstate, 1, 0, 1, 1);
  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(1, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kQueen,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(1, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kRook,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(1, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kBishop,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(1, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kKnight,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_push) != moves_generated.end());

  auto promotion_by_capture_1 = GenCaptureManually(initstate, 1, 0, 0, 1);
  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(0, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kQueen,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(0, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kRook,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(0, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kBishop,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(0, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kKnight,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_1) != moves_generated.end());

  auto promotion_by_capture_2 = GenCaptureManually(initstate, 1, 0, 2, 1);
  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(2, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kQueen,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(2, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kQueen,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(2, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kRook,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(2, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kBishop,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(2, 1,
      aithena::chess::make_piece(
        aithena::chess::Figure::kKnight,
        aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
      promotion_by_capture_2) != moves_generated.end());
}

/**
 * Test for
 *   A B C D E F G H
 * 1 - - - - - - - -
 * 2 - - - - - - - -
 * 3 - - - - - - - -
 * 4 - - - - - - - -
 * 5 - - - - - - - -
 * 6 - - - - - - - -
 * 7 - - - - - - - -
 * 8 R - - - X - - R
 * 
 * Test if castlings work
 * White's turn
 */
TEST(MoveGenerationTest, CastlingTest1) {
  aithena::chess::Game::Options options {
    {"board_width", 8},
    {"board_height", 8}
  };
  aithena::chess::Game game{options};
  aithena::chess::State initstate {
    static_cast<std::size_t>(8),
    static_cast<std::size_t>(8),
    static_cast<unsigned>(aithena::chess::Figure::kCount)
  };

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));
  board.SetField(4, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kKing, aithena::chess::Player::kWhite));
  board.SetField(7, 0, aithena::chess::make_piece(
        aithena::chess::Figure::kRook, aithena::chess::Player::kWhite));

  auto moves_generated = game.GenMoves(initstate);
  auto castling_king_side =
      GenMoveManually(GenMoveManually(initstate, 4, 0, 2, 0), 0, 0, 3, 0);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
       castling_king_side) != moves_generated.end());

  auto castling_queen_side =
      GenMoveManually(GenMoveManually(initstate, 4, 0, 6, 0), 0, 0, 5, 0);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
       castling_queen_side) != moves_generated.end());
}
