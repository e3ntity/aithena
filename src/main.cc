/*
Copyright 2020 All rights reserved.
*/

#include <memory>

#include "chess/game.h"
#include "mcts/mcts.h"

int main() {
	aithena::chess::Game::Options options = {
		{"board_width", 4},
		{"board_height", 4}
	};

	auto game = aithena::chess::Game(options);
	auto game_ptr = std::make_shared<aithena::chess::Game>(game);

	auto root = aithena::MCTSNode<aithena::chess::Game>(game_ptr);
	auto root_ptr = std::make_shared<aithena::MCTSNode<aithena::chess::Game>>(root);

	auto mcts = aithena::MCTS(game_ptr);
}
