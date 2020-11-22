#ifndef AITHENTA_GAME_ACTION
#define AITHENTA_GAME_ACTION

#include <string>

namespace aithena {

template <typename State>
class Action {
 public:
  Action(State before, State after) : before_(before), after_(after) {}

  // Returns the "before" state.
  State GetPreviousState() {
    return before_;
  }
  // Returns the "after" state.
  State GetNextState() {
    return after_;
  }

  // Returns a canonical representation of the action.
  virtual const std::string ToString() const = 0;
 private:
  // The action is represented by the state before and the state after the
  // action.
  State before_;
  State after_;
};

}

#endif // AITHENTA_GAME_ACTION
