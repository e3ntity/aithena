#include "gtest/gtest.h"

#include "board/board.h"

TEST(BoardPlane, InitializesBitfieldToCorrectSize) {
  aithena::BoardPlane bp(4, 4);

  bp.get(3, 3); // Should work
  ASSERT_DEATH(bp.get(4, 3), ".*pos < m_num_bits.*");
  ASSERT_DEATH(bp.get(3, 4), ".*pos < m_num_bits.*");
}

TEST(BoardPlane, InitializesBitfieldToFalse) {
  aithena::BoardPlane bp(4, 4);

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      ASSERT_FALSE(bp.get(x, y));
    }
  }
}

TEST(BoardPlane, SetsAndClearsCorrectly) {
  aithena::BoardPlane bp(4, 4);

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      bp.set(x, y);
      ASSERT_TRUE(bp.get(x, y));

      bp.clear(x, y);
      ASSERT_FALSE(bp.get(x, y));
    }
  }
}

TEST(BoardPlane, CopiesCorrectly) {
  aithena::BoardPlane bp(4, 4);
  bp.set(2, 2);

  aithena::BoardPlane bp2(bp);

  ASSERT_TRUE(bp2.get(2, 2));
  bp.clear(2, 2);
  ASSERT_FALSE(bp.get(2, 2));
  ASSERT_TRUE(bp2.get(2, 2));
}

TEST(BoardPlane, AndOperatorWorksCorrectly) {
  aithena::BoardPlane bp(4, 4);
  aithena::BoardPlane bp2(4, 4);

  bp.set(1, 3);
  bp2.set(3, 1);

  aithena::BoardPlane bp3 = bp | bp2;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((x == 1 && y == 3) || (x == 3 && y == 1))
        ASSERT_TRUE(bp3.get(x, y));
      else
        ASSERT_FALSE(bp3.get(x, y));
    }
  }
}

TEST(BoardPlane, OrOperatorWorksCorrectly) {
  aithena::BoardPlane bp(4, 4);
  aithena::BoardPlane bp2(4, 4);

  bp.set(0, 2);
  bp2.set(0, 2);
  bp2.set(2, 1);

  aithena::BoardPlane bp3 = bp & bp2;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (x == 0 && y == 2)
        ASSERT_TRUE(bp3.get(x, y));
      else
        ASSERT_FALSE(bp3.get(x, y));
    }
  }
}

TEST(BoardPlane, XorOperatorWorksCorrectly) {
  aithena::BoardPlane bp(4, 4);
  aithena::BoardPlane bp2(4, 4);

  bp.set(1, 3);
  bp2.set(1, 3);
  bp2.set(3, 1);

  aithena::BoardPlane bp3 = bp ^ bp2;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if ((x == 3 && y == 1))
        ASSERT_TRUE(bp3.get(x, y));
      else
        ASSERT_FALSE(bp3.get(x, y));
    }
  }
}

TEST(BoardPlane, EqualityOperatorsWorkCorrectly) {
  aithena::BoardPlane bp(4, 4);
  aithena::BoardPlane bp2(4, 4);
  aithena::BoardPlane bp3(4, 4);

  bp.set(1, 2);
  bp.set(3, 0);
  bp2.set(1, 2);
  bp2.set(3, 0);
  bp3.set(3, 0);

  ASSERT_TRUE(bp == bp2);
  ASSERT_FALSE(bp == bp3);
  ASSERT_FALSE(bp2 == bp3);

  ASSERT_FALSE(bp != bp2);
  ASSERT_TRUE(bp != bp3);
  ASSERT_TRUE(bp2 != bp3);
}

class BoardTest : public ::testing::Test {
 protected:
  void SetUp() {
    for (unsigned int i = 0; i < 9; ++i) {
      board.SetPiece(i, i, {i % 8, i % 2});
    }
  }

  aithena::Board board{9, 9, 8};
};

TEST_F(BoardTest, InitializesBitfieldToCorrectSize) {
  ASSERT_EQ(board.GetWidth(), 9);
  ASSERT_EQ(board.GetHeight(), 9);

  for (unsigned int fig = 0; fig < 8; ++fig) {
    for (unsigned int player = 0; player < 2; ++player) {
      aithena::BoardPlane bp = board.GetPlane(fig, player);

      bp.get(3, 3); // Should work
      ASSERT_DEATH(bp.get(8, 9), ".*pos < m_num_bits.*");
      ASSERT_DEATH(bp.get(9, 8), ".*pos < m_num_bits.*");
      ASSERT_DEATH(bp.get(9, 9), ".*pos < m_num_bits.*");
    }
  }
}

TEST_F(BoardTest, PieceSetCorrectly) {
  for (unsigned int i = 0; i < 9; ++i) {
    ASSERT_TRUE(board.GetPlane(i % 8, i % 2).get(i, i));
  }
}

TEST_F(BoardTest, GetPieceTest) {
  aithena::Piece piece;
  // test along the filled diagonal
  for (unsigned int i = 0; i < 9; ++i) {
    piece = board.GetPiece(i, i);
    ASSERT_EQ(piece.figure, i % 8);
    ASSERT_EQ(piece.player, i % 2);
  }

  //test squares where there are no pieces
  piece = board.GetPiece(0, 8);
  ASSERT_TRUE(piece == aithena::kEmptyPiece);

  piece = board.GetPiece(4, 2);
  ASSERT_TRUE(piece == aithena::kEmptyPiece);
}