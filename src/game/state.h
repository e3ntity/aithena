/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_GAME_STATE_H_
#define AITHENA_GAME_STATE_H_

#include <sstream>
#include <string>

#include "board/board.h"

namespace aithena {

class State {
 public:
  // Constructs the state with an empty board of given dimensions.
  State(int width, int height, unsigned char figure_count);
  State(const State&);

  State& operator=(const State&);
  bool operator==(const State&);
  bool operator!=(const State&);

  Board& GetBoard();

  // Returns a canoncial representation of the state.
  std::string ToString();

 protected:
  Board board_;
};

}  // namespace aithena

#endif  // AITHENA_GAME_STATE_H_
