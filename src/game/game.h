#ifndef AITHENTA_GAME_GAME_H
#define AITHENTA_GAME_GAME_H

#include <string>
#include <unordered_map>
#include <vector>

namespace aithena {

template <typename Action, typename State>
class Game {
 public:
  using Options = std::unordered_map<std::string, int>;
  using ActionList = std::vector<Action>;

  // Creates the game with default options.
  // Options can be added/overridden using the SetOption member function.
  Game();
  // Creates the game with custom options.
  Game(Options options);

  // Checks whether a given game option is set.
  bool HasOption(std::string option);
  // Returns the value of a given game option.
  // Take caution that the option is set!
  int GetOption(std::string option);
  // Sets a game option.
  void SetOption(std::string option, int value);

  // Returns an initial state for the game start.
  virtual State GetInitialState() = 0;
  // Computes and reutrns all legal actions given a game state.
  virtual ActionList GetLegalActions(State) = 0;

 protected:
  // Registers a new default option
  void DefaultOption(std::string option, int value);

 private:
  Options options_;
};

}

#endif // AITHENTA_GAME_GAME_H
