#include "game/action.h"

#include <string>

namespace aithena {

template <typename State>
Action<State>::Action(State before, State after) : before_(before), after_(after) {}

template <typename State>
State Action<State>::GetPreviousState() {
  return before_;
}

template <typename State>
State Action<State>::GetNextState() {
  return after_;
}

}
