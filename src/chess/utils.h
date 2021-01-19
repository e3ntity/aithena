#include <string>

#include "chess/state.h"

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker,
                             std::string marker_color = "\033[31m");

std::string PrintBoard(aithena::chess::State state);
