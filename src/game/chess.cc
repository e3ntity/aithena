#include "game/chess.h"

namespace aithena {
namespace chess {

// Initialize figures
const std::array<Figure, 6> Game::figures_ = {Figure::king, Figure::queen, Figure::rook, Figure::bishop, Figure::knight, Figure::pawn};

}
}
