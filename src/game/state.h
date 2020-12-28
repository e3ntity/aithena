/*
Copyright 2020 All rights reserved.
*/

#ifndef SRC_GAME_STATE_H_
#define SRC_GAME_STATE_H_

#include <string>
#include <sstream>

#include "board/board.h"

namespace aithena {

class State {
 public:
  // Constructs the state with an empty board of given dimensions.
  State(std::size_t width, std::size_t height, unsigned char figure_count);
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

#endif  // SRC_GAME_STATE_H_
