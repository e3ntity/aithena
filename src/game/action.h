#ifndef AITHENTA_GAME_ACTION
#define AITHENTA_GAME_ACTION

#include <string>
#include <tuple>

#include "game/state.h"

namespace aithena {

class Action {
 public:
  Action(State before, State after);

  // Returns the "before" state.
  State GetPreviousState() const;
  // Returns the "after" state.
  State GetNextState() const;

  // Returns a canonical representation of the action.
  virtual std::string ToString() const;
 private:
  // The action is represented by the state before and the state after the
  // action.
  std::tuple <State, State> transition_;
};

}

#endif // AITHENTA_GAME_ACTION
