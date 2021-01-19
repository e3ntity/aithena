#include <chess/utils.h>


std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker,
                             std::string marker_color) {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;

  std::string turn =
      state.GetPlayer() == aithena::chess::Player::kWhite ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1) ? "\033[1;47m" : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white =
          piece.player == static_cast<unsigned>(aithena::chess::Player::kWhite);
      std::string s_piece;

      // Figure icon
      switch (piece.figure) {
        case static_cast<unsigned>(aithena::chess::Figure::kKing):
          s_piece = white ? "♔" : "♚";
          break;
        case static_cast<unsigned>(aithena::chess::Figure::kQueen):
          s_piece = white ? "♕" : "♛";
          break;
        case static_cast<unsigned>(aithena::chess::Figure::kRook):
          s_piece = white ? "♖" : "♜";
          break;
        case static_cast<unsigned>(aithena::chess::Figure::kBishop):
          s_piece = white ? "♗" : "♝";
          break;
        case static_cast<unsigned>(aithena::chess::Figure::kKnight):
          s_piece = white ? "♘" : "♞";
          break;
        case static_cast<unsigned>(aithena::chess::Figure::kPawn):
          s_piece = white ? "♙" : "♟︎";
          break;
        default:
          s_piece += std::to_string(piece.figure);
          break;
      }

      bool marked = marker.get(x, y);

      if (piece == aithena::kEmptyPiece) {
        repr << s_color << (marked ? marker_color + "- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? marker_color + "-" : " ") << s_color
             << s_piece << "\033[24m" << (marked ? marker_color + "-" : " ")
             << "\033[0m";
      }
    }
  }
  repr << std::endl << "   A  B  C  D  E  F  G  H" << std::endl;

  return repr.str();
}

std::string PrintBoard(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}
