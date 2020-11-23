#include "gtest/gtest.h"

#include "board/board.h"

// Tests for BoardPlane

// Tests whether the boost::dynamic_bitset plane_ is initialized to the correct
// size by the constructor of BoardPlane.
// This must be done through a proxy (assertions) as plane_ is private.
TEST(BoardPlane, InitializesBitfieldToCorrectSize) {
  aithena::BoardPlane bp(4, 4);

  bp.get(3, 3); // Should work
  ASSERT_DEATH(bp.get(4, 3), ".*x < width_ && y < height_.*");
  ASSERT_DEATH(bp.get(3, 4), ".*x < width_ && y < height_.*");
}

// Tests whether the constructor initializes the bitfield to zero.
TEST(BoardPlane, InitializesBitfieldToFalse) {
  aithena::BoardPlane bp(4, 4);

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      ASSERT_FALSE(bp.get(x, y));
    }
  }
}

// Tests the copy constructor of BoardPlane.
TEST(BoardPlane, CopiesCorrectly) {
  aithena::BoardPlane bp(4, 4);
  bp.set(2, 2);

  aithena::BoardPlane bp2(bp);

  ASSERT_TRUE(bp2.get(2, 2));
  bp.clear(2, 2);
  ASSERT_FALSE(bp.get(2, 2));
  ASSERT_TRUE(bp2.get(2, 2));
}

// Tests BoardPlane::set and BoardPlane::get
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

// Tests the overloaded bitwise AND "&" operator
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

// Tests the overloaded bitwise OR "|" operator.
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

// Tests the overloade dbitwise XOR "^" operator.
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

// Tests the overloaded NOT "!" operator.
TEST(BoardPlane, NotOperatorWorksCorrectly) {
  aithena::BoardPlane bp(4, 4);

  bp.set(2, 2);
  bp.set(3, 1);

  aithena::BoardPlane bp2{!bp};

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      ASSERT_NE(bp.get(x, y), bp2.get(x, y));
    }
  }
}

// Tests the overloaded equality "==" and inequality "!=" operators.
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

// Tests for Board

// Tests whether the board as well as the BitPlanes planes_ are initialized
// to the correct size by the constructor.
// This must be done through a proxy (assertions) as planes_ is private.
TEST(Board, InitializesBitfieldToCorrectSize) {
  aithena::Board small_board{4, 4, 3};

  ASSERT_EQ(small_board.GetWidth(), 4);
  ASSERT_EQ(small_board.GetHeight(), 4);

  for (unsigned int fig = 0; fig < 3; ++fig) {
    for (unsigned int player = 0; player < 2; ++player) {
      aithena::BoardPlane bp = small_board.GetPlane({fig, player});

      bp.get(3, 3); // Should work
      ASSERT_DEATH(bp.get(4, 4), ".*x < width_ && y < height_.*");
      ASSERT_DEATH(bp.get(4, 3), ".*x < width_ && y < height_.*");
      ASSERT_DEATH(bp.get(3, 4), ".*x < width_ && y < height_.*");
    }
  }
}

class BoardFieldTest : public ::testing::Test {
 protected:
  void SetUp() {
    for (unsigned int i = 0; i < 9; ++i) {
      board.SetField(i, i, {i % 8, i % 2});
    }
  }

  aithena::Board board{9, 9, 8};
};

// Tests whether Board::SetField works correctly.
TEST_F(BoardFieldTest, FieldSetsCorrectly) {
  for (unsigned int i = 0; i < 9; ++i)
    ASSERT_TRUE(board.GetPlane({i % 8, i % 2}).get(i, i));
}

// Tests whether Board::GetField works correctly.
TEST_F(BoardFieldTest, FieldGetsCorrectly) {
  aithena::Piece piece;

  // test along the filled diagonal
  for (unsigned int i = 0; i < 9; ++i) {
    piece = board.GetField(i, i);
    ASSERT_EQ(piece.figure, i % 8);
    ASSERT_EQ(piece.player, i % 2);
  }

  //test squares where there are no pieces
  ASSERT_TRUE(board.GetField(0, 8) == aithena::kEmptyPiece);
  ASSERT_TRUE(board.GetField(4, 2) == aithena::kEmptyPiece);
}
