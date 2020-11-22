#include "game/game.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace aithena {

template <typename Action, typename State>
Game<Action, State>::Game() : options_{} {};

template <typename Action, typename State>
Game<Action, State>::Game(Options options) : options_{options} {};

template <typename Action, typename State>
void Game<Action, State>::DefaultOption(
  std::string option,
  int value) {
  if (HasOption(option))
    return;
  SetOption(option, value);
}

template <typename Action, typename State>
bool Game<Action, State>::HasOption(std::string option) {
  return !(options_.find(option) == options_.end());
}

template <typename Action, typename State>
int Game<Action, State>::GetOption(std::string option) {
  return options_[option];
}

template <typename Action, typename State>
void Game<Action, State>::SetOption(
  std::string option,
  int value) {
  options_[option] = value;
}

}
