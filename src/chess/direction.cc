#include "chess/direction.h"

namespace aithena {

Direction::Direction() : x{0}, y{0} {};
Direction::Direction(signed x, signed y) : x{x}, y{y} {};

Direction Direction::operator+(const Direction other) const {
	return Direction(this->x + other.x, this->y + other.y);
}

signed Direction::getX() {
	return this->x;
}

signed Direction::getY() {
	return this->y;
}
}