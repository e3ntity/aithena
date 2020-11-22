#ifndef AITHENTA_GAME_STATE_H
#define AITHENTA_GAME_STATE_H

#include "board/board.h"

namespace aithena {

class State {
 public:
  // Constructs the state with the given board.
  State(Board);
  // Constructs the state with an empty board.
  State();
  // Copy constructor.
  State(const State&);

  // Returns a canoncial representation of the state.
  virtual std::string toString();
 protected:
  Board board_;
};

}

#endif // AITHENTA_GAME_STATE_H
