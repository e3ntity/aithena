/*
Copyright 2020 All rights reserved.
*/
#include <algorithm>
#include <iostream>

#include "board/board.h"
#include "chess/game.h"
#include "chess/util.h"
#include "gtest/gtest.h"

/**
 * capital letters are white figures and small letters are black figures
 * K - King
 * Q - Queen
 * R - Rook
 * B - Bishop
 * N - Knight
 * P - Pawn
 */

using namespace aithena;

/**
 * Generates a Move in manual fashion
 */
chess::State GenMoveManually(chess::State fromState, unsigned x_start, unsigned y_start, unsigned x_dest,
                             unsigned y_dest, bool reset_no_progress = true) {
  chess::State newState{fromState};

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
  chess::Game::Options options{{"board_width", 2}, {"board_height", 6}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("k1/2/2/pP/2/1K w - a4 0 2");

  auto moves_generated = game.GenMoves(initstate);

  // 2 king moves + 1 pawn push + 1 pawn en-passant capture
  EXPECT_EQ(moves_generated.size(), 4);

  auto enpassant = GenMoveManually(*initstate, 1, 2, 0, 3);
  enpassant.GetBoard().ClearField(0, 2);

  bool found = false;
  for (auto move : moves_generated) {
    if (*move != enpassant) continue;

    found = true;
    break;
  }

  EXPECT_TRUE(found);
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
  chess::Game::Options options{{"board_width", 4}, {"board_height", 4}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("K3/2q1/1n2/3k w - - 0 1");

  auto moves_generated = game.GenMoves(initstate);
  EXPECT_EQ(moves_generated.size(), 0);
}

/**
 * Test for
 *   A B C D E
 * 5 - r b k -
 * 4 - p P p n
 * 3 p - - - -
 * 2 P - P - -
 * 1 R N B Q K
 *
 * Black king is in check
 */
TEST(MoveGenerationTest, CheckTest1) {
  chess::Game::Options options{{"board_width", 5}, {"board_height", 5}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("1rbk1/1pPpn/p4/p1P2/RNBQK b - - 0 1");

  auto moves_generated = game.GenMoves(initstate);

  EXPECT_EQ(moves_generated.size(), 2);
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
  chess::Game::Options options{{"board_width", 4}, {"board_height", 3}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("k1RK/4/R1q1 b - - 0 1");

  auto moves_generated = game.GenMoves(initstate);

  EXPECT_EQ(moves_generated.size(), 1);

  bool found = false;
  auto target_move = GenMoveManually(*initstate, 0, 2, 1, 1);
  for (auto move : moves_generated) {
    if (*move != target_move) continue;

    found = true;
    break;
  }

  EXPECT_TRUE(found);
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
  chess::Game::Options options{{"board_width", 4}, {"board_height", 4}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("1r1k/P3/4/K3 w - - 0 1");

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
  chess::Game::Options options{{"board_width", 4}, {"board_height", 4}};
  chess::Game game{options};
  chess::State::StatePtr initstate = chess::State::FromFEN("4k3/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  auto moves_generated = game.GenMoves(initstate);
  // 19 rook moves + 5 king moves + 2 castling moves = 26 moves
  EXPECT_EQ(moves_generated.size(), 26);
}

TEST(UtilityFunctionTest, TerminalStateTest) {
  chess::Game::Options options{
      {"board_width", 8}, {"board_height", 8}, {"max_no_progress", 30}, {"max_move_count", 50}};
  chess::Game game{options};

  std::tuple<std::string, int> positions[] = {
      std::make_tuple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false),
      std::make_tuple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 30 1", true),
      std::make_tuple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 50", true),
      std::make_tuple("8/8/8/8/8/2k5/1q6/K7 w - - 0 1", true)};

  for (auto position : positions) {
    auto state = chess::State::FromFEN(std::get<0>(position));

    EXPECT_EQ(game.IsTerminalState(state), static_cast<bool>(std::get<1>(position)));
  }
}

TEST(UtilityFunctionTest, ResultTest) {
  chess::Game::Options options{
      {"board_width", 8}, {"board_height", 8}, {"max_no_progress", 30}, {"max_move_count", 50}};
  chess::Game game{options};

  std::tuple<std::string, int> positions[] = {
      std::make_tuple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 30 1", 0),
      std::make_tuple("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 50", 0),
      std::make_tuple("8/8/8/8/8/2k5/1q6/K7 w - - 0 1", -1)};

  for (auto position : positions) {
    auto state = chess::State::FromFEN(std::get<0>(position));

    EXPECT_EQ(game.GetStateResult(state), std::get<1>(position));
  }
}
