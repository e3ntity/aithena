#include "game/action.h"

#include "game/state.h"

namespace aithena {

Action::Action(State before, State after) : transition_{before, after} {}

}
