#ifndef AITHENTA_GAME_CHESS_H
#define AITHENTA_GAME_CHESS_H

#include "game/action.h"
#include "game/game.h"
#include "game/state.h"

namespace aithena {
namespace chess {

enum class Figure {king, queen, rook, bishop, knight, pawn};

class Action : public ::aithena::Action {};

class Game :
  public ::aithena::Game<Figure, 6> {
 public:
  Game(Options);

  State GetInitialState() const override;
  std::vector<Action> GetLegalActions(State);
 private:
  static const std::array<Figure, 6> figures_;
};

class State : public ::aithena::State {};
}
}

#endif // AITHENTA_GAME_CHESS_H
