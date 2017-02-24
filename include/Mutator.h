#ifndef TVARI_MUTATOR
#define TVARI_MUTATOR

#include <random>

#include <Coord.h>

class Mutator
{
private:
    std::random_device              _rd;
    std::mt19937                    _gen;
    std::uniform_int_distribution<> _dis;
public:
    static Mutator *getInstance();
    Mutator(Mutator const&)             = delete;
    void operator=(Mutator const&)      = delete;
private:
    static Mutator *instance;
    Mutator();
public:
    bool   oneChanceOf(int v);
    bool   chanceToBeRemoved();
    bool   chanceToBeRedirected();
    bool   chanceToConstantChanged();
    bool   chanceToNopInserted();

    void   mutateVector(Coord &v);
    Coord  randomVector(Coord &limits);
};

#endif
