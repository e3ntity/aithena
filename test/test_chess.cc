/*
Copyright 2020 All rights reserved.
*/
#include <algorithm>
#include <iostream>

#include "gtest/gtest.h"

#include "board/board.h"
#include "chess/game.h"
#include "chess/util.h"

/**
 * capital letters are white figures and small letters are black figures
 * K - King
 * Q - Queen
 * R - Rook
 * B - Bishop
 * N - Knight
 * P - Pawn
 */

/**
 * Generates a Move in manual fashion
 */
aithena::chess::State GenMoveManually(aithena::chess::State fromState,
                                      unsigned x_start, unsigned y_start,
                                      unsigned x_dest, unsigned y_dest,
                                      bool reset_no_progress = true) {
  aithena::chess::State newState{fromState};

  newState.GetBoard().MoveField(x_start, y_start, x_dest, y_dest);
  newState.IncMoveCount();

  if (reset_no_progress) newState.ResetNoProgressCount();

  newState.SetPlayer(fromState.GetOpponent());
  newState.SetDPushPawnX(-1);
  newState.SetDPushPawnY(-1);

  return newState;
}

/**
 * Test for
 *   A B
 * 6 k -
 * 5 - -
 * 4[-]-
 * 3 p P
 * 2 - -
 * 1 - K
 *
 * Enpassant test
 * White's turn
 */
TEST(MoveGenerationTest, EnPassantTest1) {
  aithena::chess::Game::Options options{{"board_width", 2},
                                        {"board_height", 6}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate =
      *aithena::chess::State::FromFEN("k1/2/2/pP/2/1K w - a4 0 2");

  auto moves_generated = game.GenMoves(initstate);

  // 2 king moves + 1 pawn push + 1 pawn en-passant capture
  EXPECT_EQ(moves_generated.size(), 4);

  auto enpassant = GenMoveManually(initstate, 1, 2, 0, 3);
  enpassant.GetBoard().ClearField(0, 2);

  EXPECT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        enpassant) != moves_generated.end());
}

/**
 * Test for
 *   A B C D
 * 4 K - - -
 * 3 - - q -
 * 2 - n - -
 * 1 - - - k
 *
 * Check mate for player white
 * White's turn
 */
TEST(MoveGenerationTest, CheckMateTest1) {
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 4}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate =
      *aithena::chess::State::FromFEN("K3/2q1/1n2/3k w - - 0 1");

  auto moves_generated = game.GenMoves(initstate);
  EXPECT_EQ(moves_generated.size(), 0);
}

/**
 * Test for
 *   A B C D
 * 3 k - R K
 * 2 - - - -
 * 1 R - q -
 *
 * Black king is double checked hence only moving the king is possible for
 * player black. Black's turn.
 */
TEST(MoveGenerationTest, DoubleCheckTest1) {
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 3}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate =
      *aithena::chess::State::FromFEN("k1RK/4/R1q1 b - - 0 1");

  auto moves_generated = game.GenMoves(initstate);

  EXPECT_EQ(moves_generated.size(), 1);
  EXPECT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 2, 1, 1)) !=
              moves_generated.end());
}

/**
 * Test for
 *   A B C D
 * 4 - r - k
 * 3 P - - -
 * 2 - - - -
 * 1 K - - -
 *
 * Promotion by move or capture
 * White's turn
 */
TEST(MoveGenerationTest, PromotionTest1) {
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 4}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate =
      *aithena::chess::State::FromFEN("1r1k/P3/4/K3 w - - 0 1");

  auto moves_generated = game.GenMoves(initstate);

  // 4 push promotions + 4 capture promotions + 1 king move
  EXPECT_EQ(moves_generated.size(), 9);
}

/**
 * Test for
 *   A B C D E F G H
 * 8 - - - - k - - -
 * 7 - - - - - - - -
 * 6 - - - - - - - -
 * 5 - - - - - - - -
 * 4 - - - - - - - -
 * 3 - - - - - - - -
 * 2 - - - - - - - -
 * 1 R - - - K - - R
 *
 * Castling
 * White's turn
 */
TEST(MoveGenerationTest, CastleTest1) {
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 4}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate =
      *aithena::chess::State::FromFEN("4k3/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  auto moves_generated = game.GenMoves(initstate);
  // 19 rook moves + 5 king moves + 2 castling moves = 26 moves
  EXPECT_EQ(moves_generated.size(), 26);
}
