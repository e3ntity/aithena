/**
 * Copyright (C) 2020 All Rights Reserved
 */

#ifndef AITHENA_ALPHAZERO_MOVE_H_
#define AITHENA_ALPHAZERO_MOVE_H_

#include "chess/game.h"

namespace aithena {

int AZMaxChessMoveValue(int board_width, int board_height);

// Returns a unique value for each chess move. This value can be used to index
// the output nodes of the neural network.
int AZChessMoveValue(const State& before, const State& after);

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_MOVE_H_
