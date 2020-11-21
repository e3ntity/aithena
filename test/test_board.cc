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
  aithena::BoardPlane bp(8, 8);
  bp.set(2, 2);

  aithena::BoardPlane bp2(bp);

  ASSERT_TRUE(bp2.get(2, 2));
  bp.clear(2, 2);
  ASSERT_FALSE(bp.get(2, 2));
  ASSERT_TRUE(bp2.get(2, 2));
}
