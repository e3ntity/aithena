#include "game/action.h"

#include <string>

namespace aithena {

template <typename State>
Action<State>::Action(State before, State after) : before_(before), after_(after) {}

template <typename State>
const State Action<State>::GetPreviousState() const {
  return before_;
}

template <typename State>
const State Action<State>::GetNextState() const {
  return after_;
}

}
