/**
 * Copyright (C) 2020 All Right Reserved
 */

#include <torch/torch.h>
#include <iostream>

#include "alphazero/alphazero.h"
#include "chess/game.h"
#include "main.h"

using namespace aithena;

int RunAlphazero(int argc, char** argv) {
  std::cout << "Running AlphaZero" << std::endl;

  chess::Game::GamePtr game = std::make_shared<chess::Game>();
  chess::State::StatePtr state = chess::State::FromFEN(
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  AlphaZero az{game};

  az.DrawAction(state);

  return 0;
}
