#include "game/state.h"

#include <string>

namespace aithena {

State::State(std::size_t width, std::size_t height, unsigned char figure_count)
  : State(Board(width, height, figure_count)) {}

State::State(Board board) : board_{board} {}

}
