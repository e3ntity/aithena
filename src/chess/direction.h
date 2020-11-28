#ifndef AITHENA_DIRECTION_H
#define AITHENA_DIRECTION_H

namespace aithena {

class Direction {
  public:
	Direction();
	Direction(signed x, signed y);

	Direction operator+(const Direction other) const;

	signed getX();
	signed getY();
  private:
	signed x;
	signed y;
};

const Direction up{0, 1};
const Direction down{0, -1};
const Direction left{ -1, 0};
const Direction right{1, 0};
const Direction upleft{ -1, 1};
const Direction upright{1, 1};
const Direction downleft{ -1, -1};
const Direction downright{1, -1};
} // namespace aithena

#endif // AITHENA_DIRECTION_H