#include "chess/moves.h"

namespace aithena {
namespace chess {

Game::ActionList GenPawnMoves(State) {
	Game::ActionList push_moves{};
	Game::ActionList capture_moves{};

	// Remove blocked tiles

	// Handle initial move

	// TODO: handle promotion

	Game::ActionList legal_moves{};
	legal_moves.reserve(push_moves.size() + capture_moves.size());
	legal_moves.insert(legal_moves.end(), push_moves.begin(), push_moves.end());
	legal_moves.insert(legal_moves.end(), capture_moves.begin(),
	                   capture_moves.end());

	return legal_moves;
}

}
}
