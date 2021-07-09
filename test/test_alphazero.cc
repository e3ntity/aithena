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

    az_->SetSimulations(50);
  }

  chess::Game::GamePtr game_;
  std::shared_ptr<AlphaZero> az_;
};

TEST_F(AlphaZeroTest, TestNNInput) {
  auto state = chess::State::FromFEN("7k/8/8/8/8/8/8/K7 w - - 0 1");
  std::vector<AZNode::AZNodePtr> nodes = {std::make_shared<AZNode>(game_, state)};
  AZNode::AZNodePtr next_node;

  for (int i = 0; i < 10; ++i) {
    next_node = az_->DrawAction(std::make_shared<AZNode>(game_, nodes.back()->GetState()));
    next_node->SetParent(nodes.back());
    nodes.push_back(next_node);

    EXPECT_FALSE(nodes.back()->IsTerminal());
  }

  EXPECT_EQ(nodes.size(), 11);

  auto player = nodes.back()->GetState()->GetPlayer();
  torch::Tensor input = GetNNInput(nodes.back());

  EXPECT_EQ(input.sizes(), std::vector<int64_t>({1, 119, 8, 8}));

  std::reverse(nodes.begin(), nodes.end());
  for (int i = 0; i < 8; ++i) {
    torch::Tensor true_state_tensor = EncodeNodeState(nodes.at(i), player);
    torch::Tensor state_tensor = input[0].slice(0, i * 14, (i + 1) * 14);

    EXPECT_TRUE(state_tensor.equal(true_state_tensor));
  }
}

TEST_F(AlphaZeroTest, TestNNOutput) {
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
