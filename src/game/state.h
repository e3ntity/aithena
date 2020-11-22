#ifndef AITHENTA_GAME_STATE_H
#define AITHENTA_GAME_STATE_H

#include "board/board.h"

#include <string>

namespace aithena {

class State {
 public:
  // Constructs the state with an empty board of given dimensions.
  State(std::size_t width, std::size_t height, unsigned char figure_count);
  // Constructs the state with the given board.
  State(Board);

  // Returns a canoncial representation of the state.
  virtual const std::string ToString() const = 0;
 protected:
  Board board_;
};

}

#endif // AITHENTA_GAME_STATE_H
