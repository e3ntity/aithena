#include "chess/util.h"

#include <string>

#include "board/board.h"
#include "chess/game.h"

namespace aithena {
namespace chess {

std::string PrintMarkedBoard(State state, BoardPlane marker,
                             std::string marker_color) {
  Board board = state.GetBoard();
  std::ostringstream repr;
  Piece piece;
  std::vector<std::string> letters = {"A", "B", "C", "D", "E", "F", "G", "H"};

  std::string turn = state.GetPlayer() == Player::kWhite ? "White" : "Black";

  repr << turn << "'s move:";

  for (int y = board.GetHeight() - 1; y >= 0; --y) {
    repr << std::endl << y + 1 << " ";
    for (unsigned x = 0; x < board.GetWidth(); ++x) {
      // field color
      std::string s_color = ((x + y) % 2 == 1) ? "\033[1;47m" : "\033[1;45m";

      piece = board.GetField(x, y);

      // Player indication
      bool white = piece.player == static_cast<unsigned>(Player::kWhite);
      std::string s_piece;

      // Figure icon
      switch (piece.figure) {
        case static_cast<unsigned>(Figure::kKing):
          s_piece = white ? "♔" : "♚";
          break;
        case static_cast<unsigned>(Figure::kQueen):
          s_piece = white ? "♕" : "♛";
          break;
        case static_cast<unsigned>(Figure::kRook):
          s_piece = white ? "♖" : "♜";
          break;
        case static_cast<unsigned>(Figure::kBishop):
          s_piece = white ? "♗" : "♝";
          break;
        case static_cast<unsigned>(Figure::kKnight):
          s_piece = white ? "♘" : "♞";
          break;
        case static_cast<unsigned>(Figure::kPawn):
          s_piece = white ? "♙" : "♟︎";
          break;
        default:
          s_piece += std::to_string(piece.figure);
          break;
      }

      bool marked = marker.get(x, y);

      if (piece == kEmptyPiece) {
        repr << s_color << (marked ? marker_color + "- -" : "   ") << "\033[0m";
      } else {
        repr << s_color << (marked ? marker_color + "-" : " ") << s_color
             << s_piece << "\033[24m" << (marked ? marker_color + "-" : " ")
             << "\033[0m";
      }
    }
  }

  repr << std::endl << " ";
  for (int i = 0; i < int(board.GetWidth()); ++i) repr << "  " << letters.at(i);

  repr << std::endl;

  return repr.str();
}

std::string PrintBoard(State state) {
  Board board = state.GetBoard();
  BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

int perft(std::shared_ptr<Game> game, chess::State& state, int max_depth,
          int depth) {
  if (depth >= max_depth) return 0;

  int counter = 0;
  auto moves = game->GetLegalActions(state);

  for (auto move : moves) {
    counter += perft(game, move, max_depth, depth + 1) + 1;
  }

  return counter;
}

std::vector<std::tuple<State, int>> divide(std::shared_ptr<Game> game,
                                           State& state, int depth) {
  std::vector<std::tuple<State, int>> output;
  auto moves = game->GenMoves(state);

  for (auto move : moves)
    output.push_back(std::make_tuple(move, perft(game, move, depth)));

  return output;
}

}  // namespace chess
}  // namespace aithena