#ifndef AITHENTA_GAME_ACTION
#define AITHENTA_GAME_ACTION

#include "board/state.h"
#include <string>
#include <tuple>

namespace aithena {

class Action
{
public:
	Action(State before, State after) : before_after(std::make_tuple(before, after)) {}
	virtual std::string to_string();
private:
	std::tuple<State, State> before_after;
};

}

#endif // AITHENTA_GAME_ACTION
