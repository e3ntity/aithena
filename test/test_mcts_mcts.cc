#include "gtest/gtest.h"

#include <cstdio>
#include <stdlib.h>
#include <time.h>

#include "chess/game.h"
#include "mcts/node.h"
#include "mcts/mcts.h"

class MCTSTest : public ::testing::Test {
 protected:
	MCTSTest() : game_{}, root_{game_}, mcts_{game_, root_} {
		srand((unsigned)time(NULL));
	}

	using MCTSNode = aithena::MCTSNode<aithena::chess::Game>;

	void SetUp() {
		root_ = MCTSNode(game_);
	}

	aithena::chess::Game game_;
	MCTSNode root_;
	aithena::MCTS<aithena::chess::Game> mcts_;
};

TEST_F(MCTSTest, InitializesCorrectly) {
	ASSERT_TRUE(mcts_.GetRoot().GetState() == root_.GetState());
}

TEST_F(MCTSTest, SelectsUnexpandedCorrectly) {
	auto leaf = mcts_.Select(root_, mcts_.RandomSelect);

	ASSERT_TRUE(leaf.GetState() == root_.GetState());
}

TEST_F(MCTSTest, SelectsExpandedCorrectly) {
	root_.Expand();

	auto leaf = mcts_.Select(root_, mcts_.RandomSelect);

	ASSERT_TRUE(leaf.GetState() == root_.GetState());
}

TEST_F(MCTSTest, SelectsTraverseCorrectly) {
	root_.Expand();

	for (auto child : root_.GetChildren())
		child->IncDraws();

	auto leaf = mcts_.Select(root_, mcts_.RandomSelect);

	ASSERT_FALSE(leaf.GetState() == root_.GetState());

	for (auto child : root_.GetChildren())
		if (leaf.GetState() == child->GetState()) return;

	ASSERT_TRUE(false); // Loop should return before reaching here.
}
