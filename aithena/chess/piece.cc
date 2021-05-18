/**
 * Copyright (C) 2020 All Rights Reserved
 */

#include "chess/piece.h"

#include "board/board.h"

namespace aithena {
namespace chess {

Piece make_piece(Figure figure, Player player) {
  return Piece{static_cast<int>(figure), static_cast<int>(player)};
}

Player GetOpponent(Player player) {
  return player == Player::kWhite ? Player::kBlack : Player::kWhite;
}

}  // namespace chess
}  // namespace aithena
