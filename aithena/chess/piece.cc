/**
 * Copyright (C) 2020 All Rights Reserved
 */

#include "chess/piece.h"

#include <string>

#include "board/board.h"

namespace aithena {
namespace chess {

Piece make_piece(Figure figure, Player player) { return Piece{static_cast<int>(figure), static_cast<int>(player)}; }

Player GetOpponent(Player player) { return player == Player::kWhite ? Player::kBlack : Player::kWhite; }

char GetPieceSymbol(Piece piece) {
  char symbol;

  switch (piece.figure) {
    case static_cast<int>(Figure::kKing):
      symbol = 'k';
      break;
    case static_cast<int>(Figure::kQueen):
      symbol = 'q';
      break;
    case static_cast<int>(Figure::kRook):
      symbol = 'r';
      break;
    case static_cast<int>(Figure::kKnight):
      symbol = 'n';
      break;
    case static_cast<int>(Figure::kBishop):
      symbol = 'b';
      break;
    case static_cast<int>(Figure::kPawn):
      symbol = 'p';
      break;
    default:
      std::cout << "Figure: " << piece.figure << std::endl;
      assert(false);  // Invalid figure, should never reach here.
  }

  if (piece.player == Player::kWhite) symbol = std::toupper(symbol);

  return symbol;
}

}  // namespace chess
}  // namespace aithena
