#include "Defs.h"
#include "Mutator.h"

Mutator   *Mutator::instance = 0;

Mutator *Mutator::getInstance()
{
    if (!instance)
        instance = new Mutator();
    return instance;
}

Mutator::Mutator():_gen(_rd()), _dis(0, 1000000)
{
}

#define MUTATIONS_OFF 0

bool Mutator::oneChanceOf(int v)
{
    if (MUTATIONS_OFF) {
        return 0;
    }
    return _dis(_gen)%v == 0;
}

bool Mutator::chanceToBeRemoved()
{
    return oneChanceOf(MUTATIONS_LEVEL_DELETE);
}

bool Mutator::chanceToBeRedirected()
{
    return oneChanceOf(MUTATIONS_LEVEL_DIRECTION);
}

bool Mutator::chanceToConstantChanged()
{
    return oneChanceOf(MUTATIONS_LEVEL_CONSTANT);
}

bool Mutator::chanceToNopInserted()
{
    return oneChanceOf(MUTATIONS_LEVEL_INSERT);
}

void Mutator::mutateVector(Coord &v)
{
    Coord delta;
    if (v.x) {
        delta.x = _dis(_gen)%MUTATIONS_CONSTANT_MAXD;
    }
    if (v.y) {
        delta.y = _dis(_gen)%MUTATIONS_CONSTANT_MAXD;
    }
    v = v.add_raw(delta);
}

Coord Mutator::randomVector(Coord &limits)
{
    Coord cRnd(0,0);
    if (limits.x) cRnd.x = _dis(_gen)%limits.x;
    if (limits.y) cRnd.y = _dis(_gen)%limits.y;
    return cRnd;
}
