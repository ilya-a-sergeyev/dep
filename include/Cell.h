#ifndef CELL
#define CELL

#include "Instructions.h"

class Cell {
public:
    long int        tailId;
    long int        executorId;

    Instruction		instruction;
    int64_t         energy;
    Direction       dir;
	
    Cell();
    Cell(const Cell &src);
    //Cell(Cell &&src);
    Cell& operator=(const Cell& src);

    void clear();
    int64_t takeEnergy();

    std::string instruction_name(Creature *crt)
    {
        return instruction.name(crt, *this);
    }

};

#endif

