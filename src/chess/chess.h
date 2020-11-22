#ifndef AITHENTA_GAME_CHESS_H
#define AITHENTA_GAME_CHESS_H

#include "game/action.h"
#include "game/game.h"
#include "game/state.h"

namespace aithena {
namespace chess {

enum class Figure : unsigned char {
  kKing, kQueen, kRook,
  kBishop, kKnight, kPawn,
  kCount
};

enum class Player : unsigned char {kWhite, kBlack};

class State : public ::aithena::State {
 public:
  using ::aithena::State::State;
};

class Action : public ::aithena::Action<State> {
 public:
  using ::aithena::Action<State>::Action;

  State GetNextState();
 private:
  State before_;
  State after_;
};

class Game : public ::aithena::Game<Action, State> {
 public:
  Game();
  Game(Options);

  State GetInitialState() override;
  ActionList GetLegalActions(State) override;
};

}
}

#endif // AITHENTA_GAME_CHESS_H
