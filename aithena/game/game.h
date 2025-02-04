/*
Copyright 2020 All rights reserved.
*/

#ifndef AITHENA_GAME_GAME_H_
#define AITHENA_GAME_GAME_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace aithena {

template <typename State>
class Game {
 public:
  using Options = std::unordered_map<std::string, int>;
  using GameState = State;

  // Creates the game with default options.
  // Options can be added/overridden using the SetOption member function.
  Game() : options_{} {}
  // Creates the game with custom options.
  explicit Game(Options options) : options_{options} {}
  // Copy constructor
  Game(const Game<State>& other) { options_ = other.options_; }

  Game& operator=(const Game& other) {
    options_ = other.options_;

    return *this;
  }

  // Checks whether a given game option is set.
  bool HasOption(std::string option) {
    return !(options_.find(option) == options_.end());
  }
  // Returns the value of a given game option.
  // Take caution that the option is set!
  int GetOption(std::string option) { return options_[option]; }
  // Sets a game option.
  void SetOption(std::string option, int value) { options_[option] = value; }

  // Returns an initial state for the game start.
  virtual std::shared_ptr<State> GetInitialState() = 0;
  // Computes and returns all legal actions given a game state.
  virtual std::vector<std::shared_ptr<State>> GetLegalActions(
      std::shared_ptr<State>) = 0;

  // Returns whether the state is a terminal state.
  virtual bool IsTerminalState(std::shared_ptr<State>) = 0;
  // Returns the result of the state from the perspective of the player who's
  // turn it is. -1 is a loss and 0 a draw. This value will be invalid if not a
  // terminal state (check via Game::IsTerminalState).
  virtual int GetStateResult(std::shared_ptr<State>) = 0;

 protected:
  // Registers a new default option
  void DefaultOption(std::string option, int value) {
    if (HasOption(option)) return;
    SetOption(option, value);
  }

 private:
  Options options_;
};

}  // namespace aithena

#endif  // AITHENA_GAME_GAME_H_
