#ifndef COORDS
#define COORDS

#include <stdint.h>

enum Direction { Forward = 0, Back, Left, Right };

class Coord {
public:
	int32_t x;
	int32_t y;

	Coord();
	Coord(int32_t nx, int32_t ny);
    Coord next(enum Direction dir);
    Coord prev(enum Direction dir);

    Coord add(Coord vect);
	Coord add(int32_t vx, int32_t vy);

    Coord add_raw(Coord vect);
    Coord add_raw(int32_t vx, int32_t vy);

    Coord(const Coord &src) { x = src.x; y = src.y; };
	Coord(Coord &&src) { x = src.x; y = src.y; };

    Coord& operator=(const Coord& src);

    bool operator==(const Coord& src);

    unsigned length();
};

#endif
