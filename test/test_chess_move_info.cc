/*
Copyright 2020 All rights reserved.
*/
#include <iostream>

#include "gtest/gtest.h"

#include "chess/game.h"
#include "chess/util.h"

using namespace aithena;

class ChessMoveInfoTest : public ::testing::Test {
 protected:
  void SetUp() {
    chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};
    game_ = std::make_shared<chess::Game>(options);
  }

  void VerifyInfoIsSet(chess::State::StatePtr state, int depth = 1);

  std::shared_ptr<chess::Game> game_;
};

void ChessMoveInfoTest::VerifyInfoIsSet(chess::State::StatePtr state,
                                        int depth) {
  if (depth == 0) return;

  for (auto move : game_->GetLegalActions(*state)) {
    EXPECT_NE(move.move_info_, nullptr);

    VerifyInfoIsSet(std::make_shared<chess::State>(move), depth - 1);
  }
}

TEST_F(ChessMoveInfoTest, MoveInfoIsSet) {
  std::tuple<std::string, int> positions[] = {std::make_tuple(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4)};

  for (auto position : positions) {
    auto state = chess::State::FromFEN(std::get<0>(position));

    VerifyInfoIsSet(state, std::get<1>(position));
  }
}
