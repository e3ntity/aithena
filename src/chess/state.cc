#include "chess/chess.h"

namespace aithena {
namespace chess {

State::State(std::size_t width, std::size_t height, unsigned char figure_count)
  : ::aithena::State(width, height, figure_count) {}
State::State(Board board) : ::aithena::State(board) {}

State::State(const State& other) : ::aithena::State(other) {}

const std::string State::ToString() const {
  return "";
};

}
}
