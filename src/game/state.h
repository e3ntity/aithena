#ifndef AITHENTA_GAME_STATE_H
#define AITHENTA_GAME_STATE_H

#include "board/board.h"

#include <string>
#include <sstream>

namespace aithena {

class State {
 public:
  // Constructs the state with an empty board of given dimensions.
  State(std::size_t width, std::size_t height, unsigned char figure_count);
  State(const State&);

  State& operator=(const State&);

  Board& GetBoard();

  // Returns a canoncial representation of the state.
  std::string ToString();
  // Todo: Remove as only for development
  std::string Print();
 protected:
  Board board_;
};

}

#endif // AITHENTA_GAME_STATE_H
