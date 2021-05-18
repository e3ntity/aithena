/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_CHESS_MOVES_H_
#define AITHENA_CHESS_MOVES_H_

#include <vector>

#include "chess/game.h"
#include "chess/state.h"

namespace aithena {
namespace chess {

// struct that express a direction on the board
struct Direction {
  signed x;
  signed y;
};

const Direction up{0, 1};
const Direction down{0, -1};
const Direction left{-1, 0};
const Direction right{1, 0};

// Adds two Direction to generate direction that would be the additive result
// of the two Direction vectors
Direction operator+(Direction d1, Direction d2);

// Generate pseudo moves in the given directions
std::vector<State::StatePtr> GenDirectionalMoves(
    State::StatePtr state, int x, int y, std::vector<Direction> directions,
    int range);

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_MOVES_H_
