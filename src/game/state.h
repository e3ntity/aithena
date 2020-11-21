#ifndef AITHENTA_GAME_STATE_H
#define AITHENTA_GAME_STATE_H

#include "board/board.h"

namespace aithena {

class State {
 public:
  // Constructs the state with the given board.
  State(Board board);
  // Constructs the state with an empty board.
  State();

  // Returns a canoncial representation of the state.
  virtual std::string toString();
 private:
  Board boards_;
};

}

#endif // AITHENTA_GAME_STATE_H
