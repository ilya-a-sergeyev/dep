#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <vector>
#include <string>

#include "Coord.h"

enum OpCode {
              // emty cell
              Op_None=0,
              // do nothing
              Op_Nop,
              // start new creature from addres
              Op_Start,
              // clear all internal storages and flags
              Op_Reset,

              // push world cell value to the internal stack
              Op_Push,
              // pop cell value from internal stack to the world by address
              Op_Pop,
              // compare top stack value and constant
              Op_CmpTop,

              // load and store energy value of the cell  to the internal memory
              Op_GetE,
              Op_SetE,

              // internal nenory registers manipulations
              Op_Set, Op_Add, Op_Rnd,
              Op_Mov, Op_AddReg,
              Op_Check, Op_Len, Op_Cmp,
              Op_Next, Op_Prev,

              // jumps

              Op_Begin,
              Op_BreakOnErr,
              Op_ContinueOnErr,
              Op_BreakOnZ,
              Op_ContinueOnZ,
              Op_BreakOnG,
              Op_ContinueOnG,
              Op_BreakOnL,
              Op_ContinueOnL,
              Op_End,

              Op_Last
};

class Creature;
class Cell;

enum OperandType {
    Ot_None = 0,
    Ot_Constant,
    Ot_ConstantVector,  // vector given by argument
    Ot_IntMemory,       // internal memory register address
    Ot_IntMemoryPtr     // external memory address, pointer stored in the internal memory
};

struct OpOptions {
    int64_t			baseCost;
    int64_t			lengthMul;
    OperandType     targetOpType;
    OperandType     sourceOpType;
};

class Instruction {
    static std::vector<OpOptions> opOptions;
public:
    static void prepare_instructions();
    OpOptions options();

    enum OpCode		code;
	Coord			arg1;
	Coord			arg2;
	
    Instruction():code(Op_Nop) {}

    Instruction(Instruction &org):code(org.code), arg1(org.arg1), arg2(org.arg2) {}
    Instruction(Instruction &&org):code(org.code), arg1(org.arg1), arg2(org.arg2) {}
    Instruction(enum OpCode _code, Coord _arg1, Coord _arg2): code(_code),arg1(_arg1), arg2(_arg2) {}
    Instruction(enum OpCode _code, Coord _arg1): code(_code), arg1(_arg1) {}
    Instruction(enum OpCode _code): code(_code) {}

    Instruction& operator=(const Instruction& src);

    std::string name(Creature *crt, Cell &cell);
    uint64_t cost(Cell &cell, Creature &crt, bool condition);

    bool is(enum OpCode _code)
    {
        return code == _code;
    }

};


#endif
