#ifndef AITHENTA_GAME_GAME_H
#define AITHENTA_GAME_GAME_H

#include <string>
#include <unordered_map>
#include <vector>

namespace aithena {

template <typename Action, typename State>
class Game {
 public:
  using Options = std::unordered_map<std::string, unsigned int>;
  using ActionList = std::vector<Action>;

  Game();
  Game(Options options);

  // Dealing with game options
  void DefaultOption(std::string option, unsigned int value);
  bool HasOption(std::string option);
  unsigned int GetOption(std::string option);
  void SetOption(std::string option, unsigned int value);

  virtual State GetInitialState() = 0;
  virtual ActionList GetLegalActions(State) = 0;
 private:
  Options options_;
};

}

#endif // AITHENTA_GAME_GAME_H
