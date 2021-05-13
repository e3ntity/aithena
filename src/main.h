#ifndef AITHENA_MAIN_H_
#define AITHENA_MAIN_H_

#include <string>

static std::string version{"0.1.0"};

std::string GetUsageText();
std::string GetVersionText();

int RunBenchmark(int argc, char** argv);
int RunGame(int argc, char** argv);
int RunMCTS(int argc, char** argv);
int RunAlphazero(int argc, char** argv);

#endif  // AITHENA_MAIN_H_
