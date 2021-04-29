/** Test chess engine using Perft (https://www.chessprogramming.org/Perft) */

#include <iostream>

#include "gtest/gtest.h"

#include "chess/game.h"

using namespace aithena;

class ChessPerftTest : public ::testing::Test {
 protected:
  void SetUp() {
    chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};
    game_ = std::make_shared<chess::Game>(chess::Game(options));
  }

  int perft(chess::State& state, int max_dept, int dept = 0);

  std::shared_ptr<chess::Game> game_;
};

int ChessPerftTest::perft(chess::State& state, int max_depth, int depth) {
  if (depth >= max_depth) return 0;

  int counter = 0;
  auto moves = game_->GetLegalActions(state);

  for (auto move : moves) {
    counter += perft(move, max_depth, depth + 1) + 1;
  }

  return counter;
}

TEST_F(ChessPerftTest, DefaultStart1) {
  auto state = chess::State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  EXPECT_EQ(perft(*state, 1), 20);
}

TEST_F(ChessPerftTest, DefaultStart2) {
  auto state = chess::State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  EXPECT_EQ(perft(*state, 2), 420);
}

TEST_F(ChessPerftTest, DefaultStart3) {
  auto state = chess::State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  EXPECT_EQ(perft(*state, 3), 9322);
}

/*
TEST_F(ChessPerftTest, DefaultStart4) {
  auto state = chess::State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  EXPECT_EQ(perft(*state, 4), 206603);
}
*/

TEST_F(ChessPerftTest, CastleThroughCheck) {
  auto state = chess::State::FromFEN("7k/8/8/8/8/1b6/8/R3K3 w Q - 0 1");

  EXPECT_EQ(perft(*state, 1), 14);
}

TEST_F(ChessPerftTest, CastleQueenBlocked) {
  auto state = chess::State::FromFEN("6k1/8/8/8/8/8/8/R1B1K2R w KQ - 0 1");

  EXPECT_EQ(perft(*state, 1), 30);
}

TEST_F(ChessPerftTest, EnPassantDiscoveredCheck) {
  auto state = chess::State::FromFEN("7k/8/8/K1pP3q/8/8/8/8 w - c6 0 2");

  EXPECT_EQ(perft(*state, 1), 5);
}

TEST_F(ChessPerftTest, PromotionCheck) {
  auto state = chess::State::FromFEN("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");

  EXPECT_EQ(perft(*state, 2), 520);
}

// Custom test positions:
// https://gist.github.com/peterellisjones/8c46c28141c162d1d8a0f0badbc9cff9

TEST_F(ChessPerftTest, Custom1) {
  auto state = chess::State::FromFEN("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2");

  EXPECT_EQ(perft(*state, 1), 8);
}

TEST_F(ChessPerftTest, Custom2) {
  auto state = chess::State::FromFEN("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 5 3");

  EXPECT_EQ(perft(*state, 1), 8);
}

TEST_F(ChessPerftTest, Custom3) {
  auto state = chess::State::FromFEN(
      "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2");

  EXPECT_EQ(perft(*state, 1), 19);
}

TEST_F(ChessPerftTest, Custom4) {
  auto state = chess::State::FromFEN(
      "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2");

  EXPECT_EQ(perft(*state, 1), 5);
}

TEST_F(ChessPerftTest, Custom5) {
  auto state = chess::State::FromFEN(
      "2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2");

  EXPECT_EQ(perft(*state, 1), 44);
}

TEST_F(ChessPerftTest, Custom6) {
  auto state = chess::State::FromFEN(
      "rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9");

  EXPECT_EQ(perft(*state, 1), 39);
}

TEST_F(ChessPerftTest, Custom7) {
  auto state = chess::State::FromFEN("2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4");

  EXPECT_EQ(perft(*state, 1), 9);
}

TEST_F(ChessPerftTest, Custom8) {
  auto state = chess::State::FromFEN(
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

  EXPECT_EQ(perft(*state, 3), 62379);
}

TEST_F(ChessPerftTest, Custom9) {
  auto state = chess::State::FromFEN(
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
      "10");

  EXPECT_EQ(perft(*state, 3), 89890);
}

TEST_F(ChessPerftTest, Custom10) {
  auto state = chess::State::FromFEN("8/P1k5/K7/8/8/8/8/8 w - - 0 1");

  EXPECT_EQ(perft(*state, 6), 92683);
}

TEST_F(ChessPerftTest, Custom11) {
  auto state = chess::State::FromFEN("K1k5/8/P7/8/8/8/8/8 w - - 0 1");

  EXPECT_EQ(perft(*state, 6), 2217);
}

TEST_F(ChessPerftTest, Custom12) {
  auto state = chess::State::FromFEN("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1");

  EXPECT_EQ(perft(*state, 4), 23527);
}
