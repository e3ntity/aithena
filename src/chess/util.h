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

int perft(std::shared_ptr<Game> game, chess::State& state, int max_depth,
          int depth = 0);

std::vector<std::tuple<State, int>> divide(std::shared_ptr<Game> game,
                                           State& state, int depth = 1);

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_UTIL