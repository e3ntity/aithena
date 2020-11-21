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
