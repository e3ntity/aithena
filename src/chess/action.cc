#include "chess/chess.h"

#include "game/action.h"

namespace aithena {
namespace chess {

State Action::GetNextState() {
  return after_;
};

}
}
