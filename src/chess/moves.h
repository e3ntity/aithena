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
static const std::array<std::uint64_t, 64> kPawnPushes = {
	0x00000009, 0x0000000A, 0x0000000B, 0x0000000C,
	0x0000000D, 0x0000000E, 0x0000000F, 0x00000010,
	0x0000002A, 0x0000002C, 0x0000002E, 0x00000030,
	0x00000032, 0x00000034, 0x00000036, 0x00000038,
	0x00000019, 0x0000001A, 0x0000001B, 0x0000001C,
	0x0000001D, 0x0000001E, 0x0000001F, 0x00000020,
	0x00000021, 0x00000022, 0x00000023, 0x00000024,
	0x00000025, 0x00000026, 0x00000027, 0x00000028,
	0x00000029, 0x0000002A, 0x0000002B, 0x0000002C,
	0x0000002D, 0x0000002E, 0x0000002F, 0x00000030,
	0x00000031, 0x00000032, 0x00000033, 0x00000034,
	0x00000035, 0x00000036, 0x00000037, 0x00000038,
	0x00000039, 0x0000003A, 0x0000003B, 0x0000003C,
	0x0000003D, 0x0000003E, 0x0000003F, 0x00000040,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};

static const std::array<std::uint64_t, 64> kPawnCaptures = {

};

Game::ActionList GenPawnMoves(State);

}
}

#endif // AITHENA_CHESS_MOVES_H
