#ifndef AITHENTA_CHESS_GAME_H
#define AITHENTA_CHESS_GAME_H

#include "game/game.h"

namespace aithena {
namespace chess {

// Defines the figures used in chess. GhostPawn is used to keep track of
// "en-passant".
enum class Figure : unsigned {
	/*kKing, kQueen, */kRook,
	kBishop, /*kKnight, */kPawn,
	/*kGhostPawn, */kCount
};

enum class Player : unsigned {kWhite, kBlack};

} // namespace chess
} // namespace aithena

// Needs to be imported here as state needs Figure & Player
#include "chess/state.h"
#include "chess/moves.h"

namespace aithena {
namespace chess {

Piece make_piece(Figure, Player);

class Game : public ::aithena::Game<State> {
 public:
	Game();
	Game(Options);

	// Crops bitboards to the specified size of the board.
	// Bitboards are later used for legal move generation.
	void InitializeMagic();

	State GetInitialState() override;

	std::vector<State> GetLegalActions(State) override;

	// Generates all next moves for any piece at field (x, y) for a given state.
	std::vector<State> GenMoves(State, unsigned x, unsigned y);

	// Generates the next moves for a single pawn at field (x, y) for a given
	// state.
	// * Assumes that there is a pawn of the player who's turn it is at
	// field (x, y).
	// * Does not switch the player turn.
	std::vector<State> GenPawnMoves(State, unsigned x, unsigned y);

	// Generates the next moves for a single rook at field (x, y) for a given
	// state.
	// * Assumes that there is a pawn of the player who's turn it is at
	// field (x, y).
	// * Does not switch the player turn.
	std::vector<State> GenRookMoves(State state, unsigned x, unsigned y);

	// Generates the next moves for a single bishop at field (x, y) for a given
	// state.
	// * Assumes that there is a pawn of the player who's turn it is at
	// field (x, y).
	// * Does not switch the player turn.
	std::vector<State> GenBishopMoves(State state, unsigned x, unsigned y);

 private:
	// Stores the magic bit boards computed by InitializeMagic.
	// magic_bit_planes_[2*i + 0] thereby stores the push bit planes for figure i
	// whereas magic_bit_planes[2*i + 1] stores the capture bit planes
	std::array<std::unique_ptr<BoardPlane[]>,
	    static_cast<unsigned>(Figure::kCount) * 2> magic_bit_planes_;
};

}	// namespace chess
} // namespace aithena

#endif // AITHENTA_CHESS_GAME_H
