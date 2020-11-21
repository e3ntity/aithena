#include "board/board.h"

namespace aithena {

BoardPlane::BoardPlane(std::size_t width, std::size_t height)
  : width_{width}, height_{height}, plane_{width, height} {};

}
