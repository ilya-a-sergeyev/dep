#include <iomanip>
#include <iostream>

#include "World.h"

std::vector<Instruction> Adam;
std::vector<Instruction> Eva;

void Edem();

int main(int ac, char *av[])
{
    int counter = 0;
    if (ac>1) {
        std::string arg1(av[1]);
        counter = std::stoi(arg1);
    }

    Log *theLogger = Log::getInstance();
    World *theWorld = World::getInstance();

    Edem();
    theWorld->LoadCreature({10,10}, Adam, 15000);
    theWorld->LoadCreature({50,10}, Eva, 15000);
    theWorld->run(counter);

    /*
    Coord x(95,5);
    Coord y=x.add(-10,0);
    std::cout << y.x << ":" << y.y << "\n";
    y=x.add(0,-10);
    std::cout << y.x << ":" << y.y << "\n";
    y=x.add(-10,-10);
    std::cout << y.x << ":" << y.y << "\n";
    */

    delete theWorld;
    delete theLogger;
    return(0);
}

void Edem()
{
    // толстячок - избыточное количество инструкций, размножается на случайное расстояние вправо
    // требует больше стартовой энергии, но оставляет больше потомства и вообще живучий
    Adam.resize(30);

    Adam[ 0] = Instruction(Forward, Op_Reset);
    Adam[ 1] = Instruction(Forward, Op_Mark);
    Adam[ 2] = Instruction(Forward, Op_Set,   { 0,0}, {-3,0});
    Adam[ 3] = Instruction(Forward, Op_Nop);
    Adam[ 4] = Instruction(Forward, Op_Rnd,   { 1,0}, {0  ,20});
    Adam[ 5] = Instruction(Forward, Op_Add,   { 1,0}, {0  ,1});
    Adam[ 6] = Instruction(Forward, Op_Mov,   { 3,0}, {1  ,0});
    Adam[ 7] = Instruction(Forward, Op_Len,   { 2,0});
    Adam[ 8] = Instruction(Forward, Op_Nop);
    Adam[ 9] = Instruction(Forward, Op_Nop);
    Adam[10] = Instruction(Forward, Op_Distance, { 0, 0});
    Adam[11] = Instruction(Forward, Op_Mark);
    Adam[12] = Instruction(Forward, Op_Push,  {   0, 0});
    Adam[13] = Instruction(Forward, Op_Pop,   {   1, 0});
    Adam[14] = Instruction(Forward, Op_JERR,  { -14, 0});
    Adam[15] = Instruction(Forward, Op_Nop);
    Adam[16] = Instruction(Forward, Op_Nop);
    Adam[17] = Instruction(Forward, Op_Add,   { 0,0}, { 1,0});
    Adam[18] = Instruction(Forward, Op_Nop);
    Adam[19] = Instruction(Forward, Op_Add,   { 1,0}, { 1,0});
    Adam[20] = Instruction(Forward, Op_Nop);
    Adam[21] = Instruction(Forward, Op_Add,   { 2,0}, {-1,0});
    Adam[22] = Instruction(Forward, Op_Nop);
    Adam[23] = Instruction(Forward, Op_Check,    { 2, 0});
    Adam[24] = Instruction(Forward, Op_JG,       {-12,0});
    Adam[25] = Instruction(Forward, Op_Distance, { 3, 0});
    Adam[26] = Instruction(Forward, Op_Nop);
    Adam[27] = Instruction(Forward, Op_Start,    { 3, 0});
    Adam[28] = Instruction(Forward, Op_Nop);
    Adam[29] = Instruction(Forward, Op_JMP,      { -29,0});

    Eva.resize(30);

    Eva[ 0] = Instruction(Right, Op_Reset);
    Eva[ 1] = Instruction(Right, Op_Mark);
    Eva[ 2] = Instruction(Right, Op_Set,   { 0,0}, {0,-3});
    Eva[ 3] = Instruction(Right, Op_Nop);
    Eva[ 4] = Instruction(Right, Op_Rnd,   { 1,0}, {20  ,0});
    Eva[ 5] = Instruction(Right, Op_Add,   { 1,0}, {1   ,0});
    Eva[ 6] = Instruction(Right, Op_Mov,   { 3,0}, {1   ,0});
    Eva[ 7] = Instruction(Right, Op_Len,   { 2,0});
    Eva[ 8] = Instruction(Right, Op_Nop);
    Eva[ 9] = Instruction(Right, Op_Nop);
    Eva[10] = Instruction(Right, Op_Distance, { 0,0});
    Eva[11] = Instruction(Right, Op_Mark);
    Eva[12] = Instruction(Right, Op_Push,  {   0, 0});
    Eva[13] = Instruction(Right, Op_Pop,   {   1, 0});
    Eva[14] = Instruction(Right, Op_JERR,  {   0, -14});
    Eva[15] = Instruction(Right, Op_Nop);
    Eva[16] = Instruction(Right, Op_Nop);
    Eva[17] = Instruction(Right, Op_Add,   { 0,0}, { 0,1});
    Eva[18] = Instruction(Right, Op_Nop);
    Eva[19] = Instruction(Right, Op_Add,   { 1,0}, { 0,1});
    Eva[20] = Instruction(Right, Op_Nop);
    Eva[21] = Instruction(Right, Op_Add,   { 2,0}, {-1,0});
    Eva[22] = Instruction(Right, Op_Nop);
    Eva[23] = Instruction(Right, Op_Check,    { 2, 0});
    Eva[24] = Instruction(Right, Op_JG,       {0, -12});
    Eva[25] = Instruction(Right, Op_Distance, { 3, 0});
    Eva[26] = Instruction(Right, Op_Nop);
    Eva[27] = Instruction(Right, Op_Start,    { 3, 0});
    Eva[28] = Instruction(Right, Op_Nop);
    Eva[29] = Instruction(Right, Op_JMP,      { 0,-29});
}
