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
  newState.SetPlayer(newState.GetPlayer() == aithena::chess::Player::kBlack
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
                                         unsigned x_start, unsigned y_start,
                                         unsigned x_dest, unsigned y_dest) {
  aithena::chess::State newState{fromState};
  newState.GetBoard().MoveField(x_start, y_start, x_dest, y_dest);
  newState.IncMoveCount();
  newState.ResetNoProgressCount();
  newState.SetPlayer(newState.GetPlayer() == aithena::chess::Player::kBlack
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
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 4}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(4), static_cast<std::size_t>(4),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kWhite));
  board.SetField(1, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kWhite));
  board.SetField(2, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kWhite));
  board.SetField(3, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kWhite));
  board.SetField(0, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kBlack));
  board.SetField(1, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kBlack));
  board.SetField(2, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kBlack));
  board.SetField(3, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 13);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 0, 0, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 0, 0, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenCaptureManually(initstate, 0, 0, 0, 3)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 1, 0, 0, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 1, 0, 1, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 1, 0, 1, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenCaptureManually(initstate, 1, 0, 1, 3)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 1, 0, 2, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 1, 0, 3, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 2, 0, 2, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 3, 0, 3, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 3, 0, 3, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenCaptureManually(initstate, 3, 0, 3, 3)) !=
              moves_generated.end());
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
  aithena::chess::Game::Options options{{"board_width", 3},
                                        {"board_height", 5}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(3), static_cast<std::size_t>(5),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(2, 4,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kWhite));
  board.SetField(1, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kWhite));
  board.SetField(1, 2,
                 aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                            aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 4);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenCaptureManually(initstate, 1, 0, 1, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 2, 4, 1, 4)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 2, 4, 1, 3)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 2, 4, 2, 3)) !=
              moves_generated.end());
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
  aithena::chess::Game::Options options{{"board_width", 2},
                                        {"board_height", 6}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(2), static_cast<std::size_t>(5),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kPawn,
                                            aithena::chess::Player::kBlack));
  board.SetField(1, 1,
                 aithena::chess::make_piece(aithena::chess::Figure::kPawn,
                                            aithena::chess::Player::kWhite));

  auto double_push_state = game.GenMoves(initstate)[1];
  auto moves_generated = game.GenMoves(double_push_state);

  ASSERT_EQ(moves_generated.size(), 2);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(double_push_state, 0, 3, 0, 2)) !=
              moves_generated.end());
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
  aithena::chess::Game::Options options{{"board_width", 4},
                                        {"board_height", 4}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(4), static_cast<std::size_t>(4),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kWhite));
  board.SetField(1, 1,
                 aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                            aithena::chess::Player::kBlack));
  board.SetField(2, 1,
                 aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                            aithena::chess::Player::kBlack));
  board.SetField(2, 2,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kBlack));

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
  aithena::chess::Game::Options options{{"board_width", 6},
                                        {"board_height", 6}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(6), static_cast<std::size_t>(6),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 5,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kWhite));
  board.SetField(0, 4,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kWhite));
  board.SetField(0, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kBlack));
  board.SetField(5, 4,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kBlack));
  board.SetField(1, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 4);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 4, 0, 3)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 4, 0, 2)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 4, 0, 1)) !=
              moves_generated.end());
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenCaptureManually(initstate, 0, 4, 0, 0)) !=
              moves_generated.end());
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
  aithena::chess::Game::Options options{{"board_width", 6},
                                        {"board_height", 6}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(6), static_cast<std::size_t>(6),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};
  initstate.SetPlayer(aithena::chess::Player::kBlack);

  aithena::Board& board = initstate.GetBoard();
  board.SetField(0, 5,
                 aithena::chess::make_piece(aithena::chess::Figure::kKing,
                                            aithena::chess::Player::kBlack));
  board.SetField(5, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kBlack));
  board.SetField(0, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kWhite));
  board.SetField(5, 5,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kWhite));
  board.SetField(2, 2,
                 aithena::chess::make_piece(aithena::chess::Figure::kBishop,
                                            aithena::chess::Player::kBlack));
  board.SetField(3, 3,
                 aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                            aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 1);
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        GenMoveManually(initstate, 0, 5, 1, 4)) !=
              moves_generated.end());
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
  aithena::chess::Game::Options options{{"board_width", 3},
                                        {"board_height", 2}};
  aithena::chess::Game game{options};
  aithena::chess::State initstate{
      static_cast<std::size_t>(3), static_cast<std::size_t>(2),
      static_cast<unsigned>(aithena::chess::Figure::kCount)};

  aithena::Board& board = initstate.GetBoard();
  board.SetField(1, 0,
                 aithena::chess::make_piece(aithena::chess::Figure::kPawn,
                                            aithena::chess::Player::kWhite));
  board.SetField(0, 1,
                 aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                            aithena::chess::Player::kBlack));
  board.SetField(2, 1,
                 aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                            aithena::chess::Player::kBlack));

  auto moves_generated = game.GenMoves(initstate);
  ASSERT_EQ(moves_generated.size(), 12);
  auto promotion_by_push = GenMoveManually(initstate, 1, 0, 1, 1);
  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(
      1, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(
      1, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(
      1, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kBishop,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_push) != moves_generated.end());

  promotion_by_push.GetBoard().ClearField(1, 1);
  promotion_by_push.GetBoard().SetField(
      1, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_push) != moves_generated.end());

  auto promotion_by_capture_1 = GenCaptureManually(initstate, 1, 0, 0, 1);
  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(
      0, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(
      0, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(
      0, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kBishop,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_1) != moves_generated.end());

  promotion_by_capture_1.GetBoard().ClearField(0, 1);
  promotion_by_capture_1.GetBoard().SetField(
      0, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_1) != moves_generated.end());

  auto promotion_by_capture_2 = GenCaptureManually(initstate, 1, 0, 2, 1);
  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(
      2, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(
      2, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kQueen,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(
      2, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kRook,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(
      2, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kBishop,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_2) != moves_generated.end());

  promotion_by_capture_2.GetBoard().ClearField(2, 1);
  promotion_by_capture_2.GetBoard().SetField(
      2, 1,
      aithena::chess::make_piece(aithena::chess::Figure::kKnight,
                                 aithena::chess::Player::kWhite));
  ASSERT_TRUE(std::find(moves_generated.begin(), moves_generated.end(),
                        promotion_by_capture_2) != moves_generated.end());
}
