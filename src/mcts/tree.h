#ifndef AITHENTA_MCTS_TREE_H
#define AITHENTA_MCTS_TREE_H

#include <memory>

#include "game/game.h"
#include "chess/game.h"

namespace aithena {

template <typename Game>
class MCTSNode {
 public:
	using Child = std::shared_ptr<MCTSNode<Game>>;

	// Initialize with the default game state.
	MCTSNode(Game&);
	// Initialize with a specific state.
	MCTSNode(Game&, typename Game::GameState);

	// Operators
	MCTSNode<Game>& operator=(const MCTSNode& node);

	// Getter
	typename Game::GameState GetState();
	std::vector<Child> GetChildren();

	// Generates child nodes.
	void Expand();
	// Returns whether the node has all child nodes generated.
	// children_ may have members without the node being expanded if e.g. a
	// garbage collector is used to remove untouched nodes.
	bool IsExpanded();
	// Returns whether this node is a terminal node.
	bool IsTerminal();
 private:
	// This node's child nodes.
	std::vector<Child> children_;
	// An object capturing the rules of the game.
	Game& game_;
	// The state of the game that this node captures.
	typename Game::GameState state_;
	// Whether this node has all possible child nodes in children_.
	bool expanded_{false};
};

template class MCTSNode<::aithena::chess::Game>;

}

#endif // AITHENTA_MCTS_TREE_H
