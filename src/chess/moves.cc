#include "chess/moves.h"

namespace aithena {
namespace chess {
Direction operator+(Direction d1, Direction d2) {
	return {d1.x + d2.x, d1.y + d2.y};
}

std::vector<State> GenDirectionalMoves(State state, unsigned x, unsigned y,
                                       std::vector<Direction> directions,
                                       unsigned range) {
	Board board_before{state.GetBoard()};
	std::size_t width{board_before.GetWidth()};
	std::size_t height{board_before.GetHeight()};

	std::vector<State> moves{}; // Return value

	unsigned player = static_cast<unsigned>(state.GetPlayer());

	// Generate moves
	auto opponent = state.GetPlayer() == Player::kWhite
	                ? static_cast<unsigned>(Player::kBlack)
	                : static_cast<unsigned>(Player::kWhite);

	BoardPlane own_figures = board_before.GetPlayerPlane(player);
	BoardPlane enemy_figures = board_before.GetPlayerPlane(opponent);

	for (unsigned dist = 1; directions.size() && dist <= range; ++dist) {
		for (unsigned direction = 0;
		        direction < static_cast<unsigned>(directions.size()); ++direction) {
			unsigned new_x = x + dist * directions[direction].x;
			unsigned new_y = y + dist * directions[direction].y;
			if (new_x >= width || new_x < 0 || new_y >= height || new_y < 0
			        || own_figures.get(new_x, new_y)) {
				directions.erase(directions.begin() + direction);
				--direction;
				continue;
			}
			moves.push_back(State{state});
			moves.back().GetBoard().MoveField(x, y, new_x, new_y);
			if (enemy_figures.get(new_x, new_y)) {
				directions.erase(directions.begin() + direction);
			}
		}
	}

	return moves;
}
}
}
