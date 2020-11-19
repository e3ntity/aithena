#include "gtest/gtest.h"

#include "board/board.h"

TEST(BoardPlane, InitializesCorrectly) {
	aithena::BoardPlane<8, 8> bp;

	for (int i = 0; i < 64; i++)
		ASSERT_EQ(bp.get(i % 8, int(i / 8)), 0);
}

TEST(BoardPlane, SetsAndClearsCorrectly) {
	aithena::BoardPlane<8, 8> bp;

	bp.set(4, 2);
	ASSERT_EQ(bp.get(4, 2), 1);

	bp.clear(4, 2);
	ASSERT_EQ(bp.get(4, 2), 0);
}
