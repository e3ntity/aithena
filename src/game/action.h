#ifndef AITHENTA_GAME_ACTION
#define AITHENTA_GAME_ACTION

#include <string>

namespace aithena {

template <typename State>
class Action {
 public:
  Action(State before, State after);
  ~Action() = default;

  // Returns the "before" state.
  virtual State GetPreviousState();
  // Returns the "after" state.
  virtual State GetNextState();

  // Returns a canonical representation of the action.
  virtual std::string ToString() const;
 protected:
  // The action is represented by the state before and the state after the
  // action.
  State before_;
  State after_;
};

}

#endif // AITHENTA_GAME_ACTION
