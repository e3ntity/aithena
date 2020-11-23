#ifndef AITHENA_CHESS_MOVES_H
#define AITHENA_CHESS_MOVES_H

#include "chess/chess.h"

namespace aithena {
namespace chess {

// Move patterns define how a piece may move starting from a given position.
// Each entry in the array gives the pattern for the starting position at the
// index. Fields are counted from 1) left to right, 2) bottom to top.
// Example: The entry at index 10 describes the possible moves of the pawn at
// field C2.
// A zero-move means the figure cannot move in that position.
// All positions are from the viewpoint of white
static const std::array<std::uint64_t, 64> kPawnPushes = {
	0x0000000000000100, 0x0000000000000200,
	0x0000000000000400, 0x0000000000000800,
	0x0000000000001000, 0x0000000000002000,
	0x0000000000004000, 0x0000000000008000,
	0x0000000000010000, 0x0000000000020000,
	0x0000000000040000, 0x0000000000080000,
	0x0000000000100000, 0x0000000000200000,
	0x0000000000400000, 0x0000000000800000,
	0x0000000001000000, 0x0000000002000000,
	0x0000000004000000, 0x0000000008000000,
	0x0000000010000000, 0x0000000020000000,
	0x0000000040000000, 0x0000000080000000,
	0x0000000100000000, 0x0000000200000000,
	0x0000000400000000, 0x0000000800000000,
	0x0000001000000000, 0x0000002000000000,
	0x0000004000000000, 0x0000008000000000,
	0x0000010000000000, 0x0000020000000000,
	0x0000040000000000, 0x0000080000000000,
	0x0000100000000000, 0x0000200000000000,
	0x0000400000000000, 0x0000800000000000,
	0x0001000000000000, 0x0002000000000000,
	0x0004000000000000, 0x0008000000000000,
	0x0010000000000000, 0x0020000000000000,
	0x0040000000000000, 0x0080000000000000,
	0x0100000000000000, 0x0200000000000000,
	0x0400000000000000, 0x0800000000000000,
	0x1000000000000000, 0x2000000000000000,
	0x4000000000000000, 0x8000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000
};

static const std::array<std::uint64_t, 64> kPawnCaptures = {
	0x0000000000000200, 0x0000000000000500,
	0x0000000000000a00, 0x0000000000001400,
	0x0000000000002800, 0x0000000000005000,
	0x000000000000a000, 0x0000000000004000,
	0x0000000000020000, 0x0000000000050000,
	0x00000000000a0000, 0x0000000000140000,
	0x0000000000280000, 0x0000000000500000,
	0x0000000000a00000, 0x0000000000400000,
	0x0000000002000000, 0x0000000005000000,
	0x000000000a000000, 0x0000000014000000,
	0x0000000028000000, 0x0000000050000000,
	0x00000000a0000000, 0x0000000040000000,
	0x0000000200000000, 0x0000000500000000,
	0x0000000a00000000, 0x0000001400000000,
	0x0000002800000000, 0x0000005000000000,
	0x000000a000000000, 0x0000004000000000,
	0x0000020000000000, 0x0000050000000000,
	0x00000a0000000000, 0x0000140000000000,
	0x0000280000000000, 0x0000500000000000,
	0x0000a00000000000, 0x0000400000000000,
	0x0002000000000000, 0x0005000000000000,
	0x000a000000000000, 0x0014000000000000,
	0x0028000000000000, 0x0050000000000000,
	0x00a0000000000000, 0x0040000000000000,
	0x0200000000000000, 0x0500000000000000,
	0x0a00000000000000, 0x1400000000000000,
	0x2800000000000000, 0x5000000000000000,
	0xa000000000000000, 0x4000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,
	0x0000000000000000, 0x0000000000000000
};

std::vector<State> GenPawnMoves(State, Piece);
Game::ActionList GenPawnMoves(State);

}
}

#endif // AITHENA_CHESS_MOVES_H
