#include "alphazero/move.h"

#include "board/board_plane.h"
#include "chess/game.h"

namespace aithena {

#define UNDERPROMOTION_COUNT 9
#define KNIGHT_MOVE_COUNT 8

int AZMaxChessMoveValue(int board_width, int board_height) {
  // Board size (for pick up) * (Horizontal queen moves + Vertical queen moves +
  // Diagonal queen moves + 8 Knight moves + 9 Underpromotions)
  return ((board_width - 1) * 2 + (board_height - 1) * 2 +
          (std::min(board_width, board_height) - 1) * 4 + KNIGHT_MOVE_COUNT +
          UNDERPROMOTION_COUNT);
}

int AZChessMoveValue(State& before, State& after) {}

}  // namespace aithena