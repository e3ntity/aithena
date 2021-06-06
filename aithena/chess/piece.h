/**
 * Copyright (C) 2020 All Rights Reserved
 */

#ifndef AITHENA_CHESS_PIECE_H_
#define AITHENA_CHESS_PIECE_H_

#include "board/board.h"

namespace aithena {
namespace chess {

// Defines the figures used in chess.
enum class Figure : int { kKing, kQueen, kRook, kKnight, kBishop, kPawn, kCount, kInvalid };

char GetPieceSymbol(Piece);

enum Player : int { kWhite, kBlack };

Player GetOpponent(Player);

Piece make_piece(Figure, Player);

}  // namespace chess
}  // namespace aithena

#endif  // AITHENA_CHESS_PIECE_H_
