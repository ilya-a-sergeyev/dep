#ifndef WORLD
#define WORLD

#include <vector>
#include <list>

#include "Defs.h"
#include "Coord.h"
#include "Instructions.h"
#include "Creature.h"
#include "Cell.h"
#include "Log.h"

class World {
protected:
    std::vector<Cell>                           soup;
    std::list<Creature *>                       creatures;
public:

    static int32_t    worldSize;

    ~World();

    unsigned long cellIdx(uint32_t x, uint32_t y);
    Cell &getCell(unsigned long idx);
    Cell &getCell(Coord &crd);
    Cell &getCell(uint32_t x, uint32_t y);

    // loads the creature into the world
    int LoadCreature(Coord point, std::vector<Instruction> &creature, int32_t init_energy);

    // one step of creature
    int execInstruction(Creature *crt);

    void startCreature(Creature *newcomer);

    // main loop
    void run(int counter);

    static World *getInstance();
    World(World const&)             = delete;
    void operator=(World const&)    = delete;

private:
    static World *instance;
    World(int32_t size);
};


#endif
