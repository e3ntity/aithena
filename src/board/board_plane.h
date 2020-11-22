#ifndef AITHENTA_BOARD_BOARD_PLANE_H
#define AITHENTA_BOARD_BOARD_PLANE_H

#include <vector>

#include "boost/dynamic_bitset.hpp"

namespace aithena {

class BoardPlane {
 public:
	BoardPlane(std::size_t width, std::size_t height);

	// Sets the bit of the board at the specified location.
	void set(unsigned int x, unsigned int y);
	// Clears the bit of the board at the specified location.
	void clear(unsigned int x, unsigned int y);
	// Returns the bit of the board at the specified location.
	bool get(unsigned int x, unsigned int y) const;

	BoardPlane& operator=(const BoardPlane&);

	BoardPlane& operator&=(const BoardPlane&);
	BoardPlane& operator|=(const BoardPlane&);
	BoardPlane& operator^=(const BoardPlane&);

	BoardPlane operator&(const BoardPlane&);
	BoardPlane operator|(const BoardPlane&);
	BoardPlane operator^(const BoardPlane&);

	bool operator==(const BoardPlane&);
	bool operator!=(const BoardPlane&);

 private:
	std::size_t width_, height_;
	// A 2D bit plane specified rows to columns.
	// Example:
	//  plane_[5 + 2 * width_] // returns the fifth element of the second row
	boost::dynamic_bitset<> plane_;
};

}

#endif // AITHENTA_BOARD_BOARD_PLANE_H