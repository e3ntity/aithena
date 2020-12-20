#ifndef AITHENTA_CHESS_GAME_H
#define AITHENTA_CHESS_GAME_H

#include "game/game.h"

namespace aithena {
namespace chess {

// Defines the figures used in chess. GhostPawn is used to keep track of
// "en-passant".
enum class Figure : unsigned {
	kKing, kQueen, kRook,
	kKnight, kBishop, kPawn,
	/*kGhostPawn, */kCount
};

enum Player : unsigned {kWhite, kBlack};

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
	Game(const Game&);

	Game& operator=(const Game&);

	// Crops bitboards to the specified size of the board.
	// Bitboards are later used for legal move generation.
	void InitializeMagic();

	State GetInitialState() override;
	std::vector<State> GetLegalActions(State) override;

	bool IsTerminalState(State&) override;
	int GetStateResult(State&) override;

	// Generates all enxt moves for all pieces for a given state.
	// The pseudo parameter indicates whether to only generate pseudo moves.
	std::vector<State> GenMoves(State, bool pseudo = false);

	// Generates all next moves for any piece at field (x, y) for a given state.
	// The pseudo parameter indicates whether to only generate pseudo moves.
	std::vector<State> GenMoves(State, unsigned x, unsigned y,
	                            bool pseudo = false);

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
	// * Assumes that there is a rook of the player who's turn it is at
	// field (x, y).
	// * Does not switch the player turn.
	std::vector<State> GenBishopMoves(State state, unsigned x, unsigned y);

	// Generates the next moves for a single knight at field (x, y) for a given
	// state.
	// * Assumes that there is a knight of the player who's turn it is at
	// field (x, y).
	// * Does not switch the player turn.
	std::vector<State> GenKnightMoves(State state, unsigned x, unsigned y);

	// Generates the next moves for a single queen at field (x, y) for a given
	// state.
	std::vector<State> GenQueenMoves(State state, unsigned x, unsigned y);

	// Generates the next moves for a single king at field (x, y) for a given
	// state.
	std::vector<State> GenKingMoves(State state, unsigned x, unsigned y);

	// An array of all figures.
	const static std::array<Figure, 6> figures;

	// An array of all players.
	const static std::array<Player, 2> players;
 private:
	// Returns whether the player's king is in check.
	bool KingInCheck(State state, Player player);

	// Stores the magic bit boards computed by InitializeMagic.
	// magic_bit_planes_[2*i + 0] thereby stores the push bit planes for figure i
	// whereas magic_bit_planes[2*i + 1] stores the capture bit planes
	std::array<std::unique_ptr<BoardPlane[]>,
	    static_cast<unsigned>(Figure::kCount) * 2> magic_bit_planes_;
	// For faster access to GetOption("max_no_progress")
	unsigned int max_no_progress_;
};

}	// namespace chess
} // namespace aithena

#endif // AITHENTA_CHESS_GAME_H
