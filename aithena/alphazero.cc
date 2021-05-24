/**
 * Copyright (C) 2020 All Right Reserved
 */

#include "alphazero/alphazero.h"

#include <torch/torch.h>

#include <iostream>

#include "chess/game.h"
#include "chess/util.h"
#include "main.h"

using namespace aithena;

int RunAlphazero(int argc, char** argv) {
  std::cout << "Running AlphaZero" << std::endl;

  chess::Game::GamePtr game = std::make_shared<chess::Game>(
      chess::Game::Options({{"board_width", 5}, {"board_height", 5}}));
  AlphaZero az{game};

  int rounds = 100;

  for (int i = 0; i < rounds; ++i) {
    std::cout << "## Round " << i + 1 << "/" << rounds << " ##" << std::endl;
    az.SelfPlay();
  }

  chess::State::StatePtr state = game->GetInitialState();

  while (!game->IsTerminalState(state)) {
    std::cout << chess::PrintBoard(*state) << std::endl;
    state = az.DrawAction(state);
  }

  std::cout << chess::PrintBoard(*state) << std::endl;
  std::cout << "Result: " << game->GetStateResult(state) << std::endl;

  std::cout << "## Benchmark ##" << std::endl;

  for (auto entry : az.benchmark_.GetAvg(Benchmark::UNIT_MSEC)) {
    std::string name = std::get<0>(entry);
    long time = std::get<1>(entry);

    std::cout << name << ": " << time << " msec" << std::endl;
  }

  return 0;
}
