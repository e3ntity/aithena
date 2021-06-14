#ifndef AITHENA_BENCHMARK_H_
#define AITHENA_BENCHMARK_H_

#include "chess/game.h"

using namespace aithena;

int RunBenchmark(int argc, char** argv);

void RunPerftBenchmark(chess::Game::GamePtr, chess::State::StatePtr, int);

#endif  // AITHENA_BENCHMARK_H_
