
#ifndef AITHENA_ALPHAZERO_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_ALPHAZERO_H_

#include "chess/game.h"

namespace aithena {
namespace alphazero {

int MaxChessMoveValue(std::size_t board_width, std::size_t board_height);

// Returns a unique value for each chess move. This value can be used to index
// the output nodes of the neural network.
int ChessMoveValue(State& before, State& after);

}  // namespace alphazero
}  // namespace aithena

#endif  // AITHENA_ALPHAZERO_ALPHAZERO_H_
