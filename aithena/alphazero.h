#ifndef AITHENA_ALPHAZERO_H_
#define AITHENA_ALPHAZERO_H_

#include <string>

static std::string aithena_start_timestamp;

std::string GetTimestamp();

int RunAlphazero(int argc, char** argv);

#endif  // AITHENA_ALPHAZERO_H_
