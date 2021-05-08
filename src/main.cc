#include "main.h"

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

std::string GetUsageText() {
  return "Usage: ./aithena <program> <options>\n"
         "## Programs ##\n"
         "  benchmark   Benchmark the move generation\n"
         "  version     Print version information\n"
         "  game        Run an interactive game with its own prompt\n"
         "  mcts        Run MCTS\n"
         "  alphazero   Run AlphaZero\n"
         "(Run programs with \"-h\" to list available options)\n";
}

std::string GetVersionText() { return "aithena-" + version + "\n"; }

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "[!] Missing program" << std::endl;
    std::cout << GetUsageText();
    return 1;
  }

  std::string program{argv[1]};
  int args_len = argc - 1;
  char* args[args_len];

  args[0] = argv[0];
  for (int i = 2; i < argc; ++i) args[i - 1] = argv[i];

  if (program == "version") {
    std::cout << GetVersionText();
    return 0;
  }

  if (program == "benchmark") return RunBenchmark(args_len, args);
  if (program == "game") return RunGame(args_len, args);
  if (program == "mcts") return RunMCTS(args_len, args);
  if (program == "alphazero") return RunAlphazero(args_len, args);

  std::cout << "[!] Invalid program \"" << program << "\"" << std::endl;
  std::cout << GetUsageText();

  return 1;
}
