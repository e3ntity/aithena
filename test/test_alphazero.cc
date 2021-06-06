/**
 * @Copyright 2020 All Rights Reserved
 */

#include <torch/torch.h>

#include <iostream>

#include "alphazero/alphazero.h"
#include "chess/game.h"
#include "chess/util.h"
#include "gtest/gtest.h"

using namespace aithena;

class AlphaZeroTest : public ::testing::Test {
 protected:
  void SetUp() {
    chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};
    game_ = std::make_shared<chess::Game>(options);
    az_ = std::make_shared<AlphaZero>(game_);
  }

  chess::Game::GamePtr game_;
  std::shared_ptr<AlphaZero> az_;
};

TEST_F(AlphaZeroTest, TestNNOutpout) {
  auto state = chess::State::FromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  auto node = std::make_shared<AZNode>(game_, state);

  for (int i = 0; i < 1000; ++i) az_->Simulate(node);

  torch::Tensor output = GetNNOutput(node);

  for (auto child : node->GetChildren()) {
    double true_prior = static_cast<double>(child->GetVisitCount()) / static_cast<double>(node->GetVisitCount());
    double prior = GetNNOutput(output, child);

    EXPECT_NEAR(true_prior, prior, 1e-5);
  }
}
