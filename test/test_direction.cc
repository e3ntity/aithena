#include "gtest/gtest.h"

#include "chess/direction.h"

TEST(Direction, DirectionInitializesCorrectly) {
	aithena::Direction dir{2, -4};
	ASSERT_EQ(dir.getX(), 2);
	ASSERT_EQ(dir.getY(), -4);
	aithena::Direction dir2;
	ASSERT_EQ(dir2.getX(), 0);
	ASSERT_EQ(dir2.getY(), 0);
}

TEST(Direction, DirectionAddsCorrectly) {
	aithena::Direction upleft = aithena::up + aithena::left;
	ASSERT_EQ(upleft.getX(), -1);
	ASSERT_EQ(upleft.getY(), 1);
	aithena::Direction upright = aithena::up + aithena::right;
	ASSERT_EQ(upright.getX(), 1);
	ASSERT_EQ(upright.getY(), 1);
	aithena::Direction downleft = aithena::down + aithena::left;
	ASSERT_EQ(downleft.getX(), -1);
	ASSERT_EQ(downleft.getY(), -1);
	aithena::Direction downright = aithena::down + aithena::right;
	ASSERT_EQ(downright.getX(), 1);
	ASSERT_EQ(downright.getY(), -1);
	aithena::Direction none = aithena::up + aithena::down;
	ASSERT_EQ(none.getX(), 0);
	ASSERT_EQ(none.getY(), 0);
}