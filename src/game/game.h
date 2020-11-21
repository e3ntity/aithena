#ifndef AITHENTA_GAME_GAME_H
#define AITHENTA_GAME_GAME_H

#include <array>
#include <unordered_map>
#include <vector>

#include "game/action.h"
#include "game/state.h"

namespace aithena {

template <
  typename Figure,
  std::size_t figure_count
  >
class Game {
 public:
  using Options = std::unordered_map<std::string, unsigned int>;

  Game(Options);
  ~Game() = default;

  virtual State GetInitialState() const;
  virtual std::vector<Action> GetLegalActions(State) const;
 private:
  static const std::array<Figure, figure_count> figures_;
};

}

#endif // AITHENTA_GAME_GAME_H
