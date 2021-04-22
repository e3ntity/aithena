#ifndef AITHENA_CHESS_UTIL
#define AITHENA_CHESS_UTIL

#include <string>

#include "board/board.h"
#include "chess/game.h"

namespace aithena {
namespace chess {

std::string PrintMarkedBoard(State state, BoardPlane marker,
                             std::string marker_color = "\033[31m");

std::string PrintBoard(State state);

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_UTIL