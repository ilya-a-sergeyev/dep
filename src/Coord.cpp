#include "World.h"

//
// ==========================================
// Vector math in toroidal world
// ==========================================
//

Coord Coord::next(enum Direction dir)
{
	Coord newcoord;
	newcoord.x = x;
	newcoord.y = y;
	switch (dir & 3) {
		case Forward: {
            newcoord.x = (newcoord.x+1) % World::worldSize;
			// next row
            newcoord.y = newcoord.x?newcoord.y:(newcoord.y+1)%World::worldSize;
			break;
		}
		case Right: {
            newcoord.y = (newcoord.y+1) % World::worldSize;
			// next column
            newcoord.x = newcoord.y?newcoord.x:(newcoord.x+1)%World::worldSize;
			break;
		}
		case Back: {
			if (x > 0)
				newcoord.x = x-1;
			else
                newcoord.x = World::worldSize-1;
			break;
		}
		case Left: {
			if (y > 0)
				newcoord.y = y-1;
			else
                newcoord.y = World::worldSize-1;
			break;
		}
	}
	return newcoord;
}

Coord Coord::prev(enum Direction dir)
{
    Coord newcoord;
    newcoord.x = x;
    newcoord.y = y;
    switch (dir & 3) {
        case Back: {
            newcoord.x = (newcoord.x+1) % World::worldSize;
            // next row
            newcoord.y = newcoord.x?newcoord.y:(newcoord.y+1)%World::worldSize;
            break;
        }
        case Left: {
            newcoord.y = (newcoord.y+1) % World::worldSize;
            // next column
            newcoord.x = newcoord.y?newcoord.x:(newcoord.x+1)%World::worldSize;
            break;
        }
        case Forward: {
            if (x > 0)
                newcoord.x = x-1;
            else
                newcoord.x = World::worldSize-1;
            break;
        }
        case Right: {
            if (y > 0)
                newcoord.y = y-1;
            else
                newcoord.y = World::worldSize-1;
            break;
        }
    }
    return newcoord;
}

Coord Coord::add(Coord vect)
{
	return add(vect.x, vect.y);
}

Coord Coord::add(int32_t vx, int32_t vy)
{
	Coord newcoord;

    newcoord.x = (x+vx)%World::worldSize;
    newcoord.y = y;

    if (x+vx >= World::worldSize) {
        newcoord.y = (y+vx/World::worldSize+1)%World::worldSize;
    }
    else if (x+vx<0) {
        newcoord.y = (y-vx/World::worldSize-1)%World::worldSize;
        newcoord.x += World::worldSize;
    }

    int32_t ty = newcoord.y;
    newcoord.y = (ty+vy)%World::worldSize;

    if (y+vy >= World::worldSize) {
        newcoord.x = (newcoord.x+vy/World::worldSize+1)%World::worldSize;
    }
    else if (y+vy<0) {
        newcoord.x = (newcoord.x-vy/World::worldSize-1)%World::worldSize;
        newcoord.y += World::worldSize;
    }

    /*
    if (ty+vy > World::worldSize) {
        newcoord.x = newcoord.x+vy/World::worldSize+1;
    }
    if (ty+vy == World::worldSize) {
        newcoord.x = newcoord.x+vy/World::worldSize;
    }
    else if (ty+vy<0) {
        newcoord.x = newcoord.x-vy/World::worldSize-1;
        newcoord.y += World::worldSize;
    }
    */

	return newcoord;
}

Coord Coord::add_raw(Coord vect)
{
    return add_raw(vect.x, vect.y);
}

Coord Coord::add_raw(int32_t vx, int32_t vy)
{
    Coord newcoord;
    newcoord.x = (x+vx)%World::worldSize;
    newcoord.y = (y+vy)%World::worldSize;
    return newcoord;
}

Coord::Coord()
{
	x=0;
	y=0;
}

Coord::Coord(int32_t nx, int32_t ny)
{
    //x=nx%World::worldSize;
    //y=ny%World::worldSize;
    x=nx;
    y=ny;
}

Coord& Coord::operator=(const Coord& src)
{
	if (this == &src) {
		return *this;
	}
	x = src.x;
	y = src.y;
	return *this;	
}

bool Coord::operator==(const Coord& src)
{
    return x==src.x && y==src.y;
}

static unsigned sqrt_newton(long l)
{
      long temp , div;
      unsigned  rslt = (unsigned)l;
      if (l <=  0)
            return 0;
      else if (l & 0xFFFF0000L)
            if  (l & 0xFF000000L)
                  div  = 0x3FFF;
            else
                  div  = 0x3FF;
      else
            if  (l & 0x0FF00L)
                  div  = 0x3F;
            else
                  div  = (l > 4) ? 0x7 : l;
      while (1)
      {
            temp = l  / div + div;
            div =  temp >>  1;
            div += temp  & 1;
            if  (rslt > div) {
                   rslt = (unsigned)div;
            }
            else
            {
                  if (l/rslt == rslt-1 && l%rslt == 0)
                        rslt--;
                  return rslt;
            }
      }
}

unsigned Coord::length()
{
    if (y == 0) {
        return std::abs(x);
    }
    if (x == 0) {
        return std::abs(y);
    }
    return sqrt_newton(x*x+y*y);
}

void Coord::inc(Direction dir)
{
    switch (dir) {
        case Forward: {
            x++;
            break;
        }
        case Back:{
            x--;
            break;
        }
        case Left:{
            y--;
            break;
        }
        case Right:{
            y++;
            break;
        }
    }
}

void Coord::dec(Direction dir)
{
    switch (dir) {
        case Forward: {
            x--;
            break;
        }
        case Back:{
            x++;
            break;
        }
        case Left:{
            y++;
            break;
        }
        case Right:{
            y--;
            break;
        }
    }
}

