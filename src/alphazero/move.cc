#include "alphazero/move.h"

#include "board/board_plane.h"
#include "chess/game.h"

namespace aithena {
namespace alphazero {

#define UNDERPROMOTION_COUNT 9
#define KNIGHT_MOVE_COUNT 8

int MaxChessMoveValue(std::size_t board_width, std::size_t board_height) {
  // Board size (for pick up) * (Horizontal queen moves + Vertical queen moves +
  // Diagonal queen moves + 8 Knight moves + 9 Underpromotions)
  return board_width * board_height *
         ((board_width - 1) * 2 + (board_height - 1) * 2 +
          (std::min(board_width, board_height) - 1) * 4 + KNIGHT_MOVE_COUNT +
          UNDERPROMOTION_COUNT);
}

int ChessMoveValue(State& before, State& after) {
  std::size_t width = before.GetBoard().GetWidth();
  std::size_t height = before.GetBoard().GetHeight();

  BoardPlane plane_from = before.GetBoard().GetCompletePlane() &
                          GetNewFields(after.GetBoard(), before.GetBoard());
  BoardPlane plane_to = after.GetBoard().GetCompletePlane() &
                        GetNewFields(before.GetBoard(), after.GetBoard());

  int from[2], to[2];

  for (int x = 0; x < int(width); ++x) {
    for (int y = 0; y < int(height); ++y) {
      if (plane_from.get(x, y)) {
        from[0] = x;
        from[1] = y;
      }
      if (plane_to.get(x, y)) {
        to[0] = x;
        to[1] = y;
      }
    }
  }

  // For computation, see MaxChessMoveValue function.
  int pick_up_term =
      (from[0] * width + from[1]) *
      ((width - 1) * 2 + (height - 1) * 2 + (std::min(width, height) - 1) * 4 +
       KNIGHT_MOVE_COUNT + UNDERPROMOTION_COUNT);

  Piece piece_before = before.GetBoard().GetField(from[0], from[1]);
  Piece piece_after = after.GetBoard().GetField(to[0], to[1]);

  int move_term = 0;

  do {
    int dx = to[0] - from[0];
    int dy = to[1] - from[1];

    if (piece_before.figure != piece_after.figure &&
        piece_after.figure != static_cast<unsigned>(chess::Figure::kQueen)) {
      assert(piece_before.figure ==
             static_cast<unsigned>(chess::Figure::kPawn));
      assert(piece_after.figure != static_cast<unsigned>(chess::Figure::kKing));
      assert(piece_after.figure != static_cast<unsigned>(chess::Figure::kPawn));

      // Underpromotion
      if (dx < 0) move_term += 3;
      if (dx > 0) move_term += 6;

      switch (piece_after.figure) {
        case static_cast<unsigned>(chess::Figure::kRook):
          break;
        case static_cast<unsigned>(chess::Figure::kKnight):
          move_term += 1;
          break;
        case static_cast<unsigned>(chess::Figure::kBishop):
          move_term += 2;
          break;
        default:
          assert(false);
      }
      break;
    } else {
      move_term += 9;
    }

    // Queen or knight move

    if (dy == 0) {
      // Horizontal queen move
      move_term += dx > 0 ? dx + width - 2 : dx + width - 1;
      break;
    } else {
      move_term += (width - 1) * 2;
    }

    if (dx == 0) {
      // Vertical queen move
      move_term += dy > 0 ? dy + height - 2 : dy + height - 1;
      break;
    } else {
      move_term += (height - 1) * 2;
    }

    int diagonal = std::min(width, height) - 1;

    if (abs(dx) == abs(dy)) {
      // Diagonal queen move
      if (dx > 0) move_term += diagonal;
      if (dy > 0) move_term += diagonal;
      if (dx > 0 && dy < 0) move_term += 2 * diagonal;

      move_term += abs(dx);
      break;
    } else {
      move_term += diagonal * 4;
    }

    // Knight move
    if (dx > 0) move_term += 2;
    if (dy > 0) move_term += 2;
    if (dx > 0 && dy < 0) move_term += 4;

    move_term += abs(dx) - 1;
    break;
  } while (false);

  return pick_up_term + move_term;
}

}  // aithena
}  // alphazero