
#ifndef AITHENA_ALPHAZERO_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_ALPHAZERO_H_

#include "chess/game.h"

namespace aithena {

int AZMaxChessMoveValue(int board_width, int board_height);

// Returns a unique value for each chess move. This value can be used to index
// the output nodes of the neural network.
int AZChessMoveValue(State& before, State& after);

}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO_H_
