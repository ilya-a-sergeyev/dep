#include "Defs.h"
#include "Cell.h"

Cell& Cell::operator=(const Cell& src)
{
    if (this == &src) {
        return *this;
    }
    instruction = src.instruction;
    energy = src.energy;
    tailId = src.tailId;
    executorId = src.executorId;
    return *this;
}

int64_t Cell::takeEnergy()
{
    int64_t ret = energy;
    energy = 0;
    return ret;
}

Cell::Cell()
{
    instruction.code = Op_None;
    //instruction.dir = static_cast<Direction>(std::rand()%4);
    dir = static_cast<Direction>(std::rand()%4);
    energy = INIT_CELL_ENERGY;
    tailId = 0;
    executorId = 0;
}

Cell::Cell(const Cell &src)
{
    instruction = src.instruction;
    energy = src.energy;
    tailId = 0;
    executorId = 0;
}

/*
Cell::Cell(Cell &&src)
{
    instruction = src.instruction;
    energy = src.energy;
    tailId = 0;
    executorId = 0;
}
*/

// We must not change the direction!
void Cell::clear()
{
    energy = INIT_CELL_ENERGY;
    instruction.code = Op_None;
    tailId = 0;
    executorId = 0;
}
