#ifndef AITHENTA_GAME_CHESS_H
#define AITHENTA_GAME_CHESS_H

#include "game/action.h"
#include "game/game.h"
#include "game/state.h"

namespace aithena {
namespace chess {

enum class Figure : unsigned char {
  /*kKing, kQueen, kRook,
  kBishop, kKnight, */kPawn,
  kCount
};

enum class Player : unsigned char {kWhite, kBlack};

class State : public ::aithena::State {
 public:
  State(std::size_t width, std::size_t height, unsigned char figure_count);
  State(Board);
  State(const State&);

  const std::string ToString() const override;
};

class Action : public ::aithena::Action<State> {
 public:
  using ::aithena::Action<State>::Action;

  const std::string ToString() const override;
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
