/**
 * Copyright (C) 2020 All Right Reserved
 */

#ifndef AITHENA_CHESS_UTIL_H_
#define AITHENA_CHESS_UTIL_H_

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "board/board.h"
#include "chess/game.h"

namespace aithena {
namespace chess {

std::string PrintMarkedBoard(State state, BoardPlane marker, std::string marker_color = "\033[31m");

std::string PrintBoard(State state);

int perft(std::shared_ptr<Game> game, chess::State::StatePtr state, int depth = 1);

std::vector<std::tuple<State::StatePtr, int>> divide(std::shared_ptr<Game> game, State::StatePtr state, int depth = 1);

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_UTIL_H_
