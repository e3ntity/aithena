/** Test chess engine using Perft (https://www.chessprogramming.org/Perft) */

#include <iostream>

#include "gtest/gtest.h"

#include "chess/game.h"

using namespace aithena;

std::string PrintMarkedBoard(aithena::chess::State state,
                             aithena::BoardPlane marker,
                             std::string marker_color = "\033[31m") {
  aithena::Board board = state.GetBoard();
  std::ostringstream repr;
  aithena::Piece piece;
  std::vector<std::string> letters = {"A", "B", "C", "D", "E", "F", "G", "H"};

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

  repr << std::endl << " ";
  for (int i = 0; i < int(board.GetWidth()); ++i) repr << "  " << letters.at(i);

  repr << std::endl;

  return repr.str();
}

std::string PrintBoard(aithena::chess::State state) {
  aithena::Board board = state.GetBoard();
  aithena::BoardPlane marker{board.GetWidth(), board.GetHeight()};

  return PrintMarkedBoard(state, marker);
}

chess::State GetStartState() {
  chess::State start_state = chess::State(8, 8, 6);
  Board& start_board = start_state.GetBoard();

  chess::Player players[2] = {chess::Player::kWhite, chess::Player::kBlack};

  // Setup classic chess start
  for (int p = 0; p < 2; ++p) {
    start_board.SetField(0, p * 7,
                         chess::make_piece(chess::Figure::kRook, players[p]));
    start_board.SetField(1, p * 7,
                         chess::make_piece(chess::Figure::kKnight, players[p]));
    start_board.SetField(2, p * 7,
                         chess::make_piece(chess::Figure::kBishop, players[p]));
    start_board.SetField(3, p * 7,
                         chess::make_piece(chess::Figure::kQueen, players[p]));
    start_board.SetField(4, p * 7,
                         chess::make_piece(chess::Figure::kKing, players[p]));
    start_board.SetField(5, p * 7,
                         chess::make_piece(chess::Figure::kBishop, players[p]));
    start_board.SetField(6, p * 7,
                         chess::make_piece(chess::Figure::kKnight, players[p]));
    start_board.SetField(7, p * 7,
                         chess::make_piece(chess::Figure::kRook, players[p]));

    for (int i = 0; i < 8; ++i) {
      start_board.SetField(i, p * 5 + 1,
                           chess::make_piece(chess::Figure::kPawn, players[p]));
    }
  }

  return start_state;
}

int perft(std::shared_ptr<chess::Game> game, chess::State& state, int max_depth,
          int depth = 0) {
  if (depth >= max_depth) return 0;

  int counter = 0;
  auto moves = game->GetLegalActions(state);

  for (auto move : moves) {
    counter += perft(game, move, max_depth, depth + 1) + 1;
  }

  return counter;
}

TEST(ChessPerft, Depth1) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 1), 20);
}

TEST(ChessPerft, Depth2) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 2), 420);
}

TEST(ChessPerft, Depth3) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 3), 9322);
}

TEST(ChessPerft, Depth4) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 4), 206603);
}

TEST(ChessPerft, Depth5) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 5), 5072212);
}

TEST(ChessPerft, Depth6) {
  chess::Game::Options options = {{"board_width", 8}, {"board_height", 8}};

  auto game = chess::Game(options);
  auto game_ptr = std::make_shared<chess::Game>(game);

  auto start_state = GetStartState();

  ASSERT_EQ(perft(game_ptr, start_state, 6), 124132536);
}
