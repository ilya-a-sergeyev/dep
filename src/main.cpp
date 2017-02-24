#include <iomanip>
#include <iostream>

#include "World.h"

Model Adam;
Model Eve;
Model SmartAss;

void Edem();

int main(int ac, char *av[])
{
    int counter = 0;
    if (ac>1) {
        std::string arg1(av[1]);
        counter = std::stoi(arg1);
    }

    //Instruction I1 = Instruction(Forward, Op_Set,   { 4,0}, {0  , -1});
    //std::cout << I1.arg2.x << ":" << I1.arg2.y << "\n";
    //Instruction I2 = Instruction(Forward, Op_Set,   { 4,0}, {10000  , 0});
    //std::cout << I2.arg2.x << ":" << I2.arg2.y << "\n";
    //Coord A = I1.arg2;
    //std::cout << A.x << ":" << A.y << "\n";
    //return 0;

    Log *theLogger = Log::getInstance();
    Mutator *theMutator = Mutator::getInstance();
    World *theWorld = World::getInstance();

    Edem();
    theWorld->LoadCreature({10,10}, Adam, 15000);
    theWorld->LoadCreature({50,10}, Eve, 15000);
    theWorld->LoadCreature({10,20}, SmartAss, 15000);
    theWorld->run(counter);

    //Coord x(95,5);
    //Coord y=x.add(-10,0);
    //std::cout << y.x << ":" << y.y << "\n";
    //y=x.add(0,-10);
    //std::cout << y.x << ":" << y.y << "\n";
    //y=x.add(-10,-10);
    //std::cout << y.x << ":" << y.y << "\n";

    delete theWorld;
    delete theLogger;
    return(0);
}

void Edem()
{
    // New generation
    Adam.resize(18);

    Adam[ 0] = { Forward, Instruction(Op_Begin)}; // (*)
    Adam[ 1] = { Forward, Instruction(Op_Reset)};
    // get the target start point into (4) - random(10)+1
    // - will be used in "Pop" instruction
    Adam[ 2] = { Forward, Instruction(Op_Rnd,      { 4, 0 }, {  0, 10 } ) };
    Adam[ 3] = { Forward, Instruction(Op_Add,      { 4, 0 }, {  0,  1 } ) };
    // store start point into (3) for start instruction
    Adam[ 4] = { Forward, Instruction(Op_Mov,      { 3, 0 }, {  4,  0 } ) };
    // (2) - copy loop counter
    Adam[ 5] = { Forward, Instruction(Op_Len,      { 2, 0 } ) };
    // get the source start point into (0) - will be used in "Push" instruction
    Adam[ 6] = { Forward, Instruction(Op_Set,      { 0, 0 }, { 0, 0 } ) };
    // body copy loop
    Adam[ 7] = { Forward, Instruction(Op_Begin)};
    Adam[ 8] = { Forward, Instruction(Op_Check,    { 2, 0 } ) };
    Adam[ 9] = { Forward, Instruction(Op_BreakOnZ)};
    Adam[10] = { Forward, Instruction(Op_Push,     { 0, 0 }, { 1, 0 }) }; // second operand = 1
    Adam[11] = { Forward, Instruction(Op_Pop,      { 4, 0 }, { 1, 0 }) }; // it means relative to the first "Begin" (*)
    Adam[12] = { Forward, Instruction(Op_BreakOnErr)};
    Adam[13] = { Forward, Instruction(Op_Add,      { 2, 0 }, { -1, 0 } ) };
    Adam[14] = { Forward, Instruction(Op_End)};
    // restart if copying failed
    Adam[15] = { Forward, Instruction(Op_ContinueOnErr)};
    // start the child
    Adam[16] = { Forward, Instruction(Op_Start,    { 3, 0 }, { 0, 0 } ) };
    Adam[17] = { Forward, Instruction(Op_End) };

    Eve.resize(18);

    Eve[ 0] = { Right, Instruction(Op_Begin)};
    Eve[ 1] = { Right, Instruction(Op_Reset)};
    // get the target start point
    Eve[ 2] = { Right, Instruction(Op_Rnd,      { 4, 0 }, { 10,  0 } ) };
    Eve[ 3] = { Right, Instruction(Op_Add,      { 4, 0 }, {  1,  0 } ) };
    Eve[ 4] = { Right, Instruction(Op_Mov,      { 3, 0 }, {  4,  0 } ) };
    // copy loop counter
    Eve[ 5] = { Right, Instruction(Op_Len,      { 2, 0 } ) };
    // get the source start point
    Eve[ 6] = { Right, Instruction(Op_Set,      { 0, 0 }, { 0, 0 } ) };
    // body copy loop
    Eve[ 7] = { Right, Instruction(Op_Begin)};
    Eve[ 8] = { Right, Instruction(Op_Check,    { 2, 0 } ) };
    Eve[ 9] = { Right, Instruction(Op_BreakOnZ)};
    Eve[10] = { Right, Instruction(Op_Push,     { 0, 0 }, { 1, 0 }) };
    Eve[11] = { Right, Instruction(Op_Pop,      { 4, 0 }, { 1, 0 }) };
    Eve[12] = { Right, Instruction(Op_BreakOnErr)};
    Eve[13] = { Right, Instruction(Op_Add,      { 2, 0 }, { -1, 0 } ) };
    Eve[14] = { Right, Instruction(Op_End)};
    // restart if copying failed
    Eve[15] = { Right, Instruction(Op_ContinueOnErr)};
    // start the child
    Eve[16] = { Right, Instruction(Op_Start,    { 3, 0 }, { 0, 0 } ) };
    Eve[17] = { Right, Instruction(Op_End) };


    SmartAss.resize(35);

    SmartAss[ 0] = { Right, Instruction(Op_Begin)};
    SmartAss[ 1] = { Right, Instruction(Op_Reset)};
    // get the target start point
    SmartAss[ 2] = { Right, Instruction(Op_Rnd,      { 4, 0 }, { 10,  0 } ) };
    SmartAss[ 3] = { Right, Instruction(Op_Add,      { 4, 0 }, {  1,  0 } ) };
    SmartAss[ 4] = { Right, Instruction(Op_Mov,      { 3, 0 }, {  4,  0 } ) };
    // copy loop counter
    SmartAss[ 5] = { Right, Instruction(Op_Len,      { 2, 0 } ) };

    // store start point
    SmartAss[ 6] = { Right, Instruction(Op_Mov,      { 6, 0 }, {  4, 0 }) };

    // breeding area analyse block - receives the amount of energy of the breeding area

    SmartAss[ 7] = { Right, Instruction(Op_Set,      { 5, 0 }, {  0, 0 }) };  // E sum
    // target place energy check loop
    SmartAss[ 8] = { Right, Instruction(Op_Begin)};
    SmartAss[ 9] = { Right, Instruction(Op_Check,    { 2, 0 } ) };
    SmartAss[10] = { Right, Instruction(Op_BreakOnZ)};
    SmartAss[11] = { Right, Instruction(Op_Push,     { 4, 0 }, { 1,  0 }) };  // memory -> stack
    SmartAss[12] = { Right, Instruction(Op_GetE,     { 1, 0 }) };             // stack -> register (x = energy)
    SmartAss[13] = { Right, Instruction(Op_Pop,      { 4, 0 }, { -1, 0 }) };  // -1 means Pop without writing to cell
    SmartAss[14] = { Right, Instruction(Op_BreakOnErr)};
    SmartAss[15] = { Right, Instruction(Op_AddReg,   { 5, 0 }, {  1, 0 }) };
    SmartAss[16] = { Right, Instruction(Op_Add,      { 2, 0 }, { -1, 0 }) };
    SmartAss[17] = { Right, Instruction(Op_End)};

    // unseccessfull check -> repeat from begin
    SmartAss[18] = { Right, Instruction(Op_ContinueOnErr)};
    SmartAss[19] = { Right, Instruction(Op_Cmp,   { 5, 0 }, {  30*INIT_CELL_ENERGY, 0 }) };
    SmartAss[20] = { Right, Instruction(Op_ContinueOnL)};

    // restore successfull target start point
    SmartAss[21] = { Right, Instruction(Op_Mov,      { 4, 0 }, {  6, 0 }) };
    SmartAss[22] = { Right, Instruction(Op_Len,      { 2, 0 } ) };

    // get the source start point
    SmartAss[23] = { Right, Instruction(Op_Set,      { 0, 0 }, { 0, 0 } ) };
    // body copy loop
    SmartAss[24] = { Right, Instruction(Op_Begin)};
    SmartAss[25] = { Right, Instruction(Op_Check,    { 2, 0 } ) };
    SmartAss[26] = { Right, Instruction(Op_BreakOnZ)};
    SmartAss[27] = { Right, Instruction(Op_Push,     { 0, 0 }, { 1, 0 }) };
    SmartAss[28] = { Right, Instruction(Op_Pop,      { 4, 0 }, { 1, 0 }) };
    SmartAss[29] = { Right, Instruction(Op_BreakOnErr)};
    SmartAss[30] = { Right, Instruction(Op_Add,      { 2, 0 }, { -1, 0 } ) };
    SmartAss[31] = { Right, Instruction(Op_End)};
    // restart if copying failed
    SmartAss[32] = { Right, Instruction(Op_ContinueOnErr)};
    // start the child
    SmartAss[33] = { Right, Instruction(Op_Start,    { 3, 0 }, { 0, 0 } ) };
    SmartAss[34] = { Right, Instruction(Op_End) };
}


