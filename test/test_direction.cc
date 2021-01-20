#include "gtest/gtest.h"

#include "chess/moves.h"

TEST(Direction, DirectionInitializesCorrectly) {
  aithena::chess::Coordinate dir{2, -4};
  ASSERT_EQ(dir.x, 2);
  ASSERT_EQ(dir.y, -4);
  aithena::chess::Coordinate dir2{0, 0};
  ASSERT_EQ(dir2.x, 0);
  ASSERT_EQ(dir2.y, 0);
}

TEST(Direction, DirectionAddsCorrectly) {
  aithena::chess::Coordinate upleft = aithena::chess::up
                                     + aithena::chess::left;
  ASSERT_EQ(upleft.x, -1);
  ASSERT_EQ(upleft.y, 1);
  aithena::chess::Coordinate upright = aithena::chess::up
                                      + aithena::chess::right;
  ASSERT_EQ(upright.x, 1);
  ASSERT_EQ(upright.y, 1);
  aithena::chess::Coordinate downleft = aithena::chess::down
                                       + aithena::chess::left;
  ASSERT_EQ(downleft.x, -1);
  ASSERT_EQ(downleft.y, -1);
  aithena::chess::Coordinate downright = aithena::chess::down
                                        + aithena::chess::right;
  ASSERT_EQ(downright.x, 1);
  ASSERT_EQ(downright.y, -1);
  aithena::chess::Coordinate none = aithena::chess::up + aithena::chess::down;
  ASSERT_EQ(none.x, 0);
  ASSERT_EQ(none.y, 0);
}
