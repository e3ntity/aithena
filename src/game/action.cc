#include <stdexcept>

#include "game/action.h"

#include "game/state.h"

namespace aithena {

Action::Action(State before, State after) : transition_{before, after} {}

State Action::GetPreviousState() const {
	return std::get<0>(transition_);
}

State Action::GetNextState() const {
	return std::get<1>(transition_);
}

}
