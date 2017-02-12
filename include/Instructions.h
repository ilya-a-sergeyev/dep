#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <vector>
#include <string>

#include "Coord.h"

enum OpCode { Op_None=0, Op_Nop, Op_Start, Op_Reset,

              Op_Mark, Op_Distance,

              Op_Push, Op_Pop, Op_CmpTop,

              Op_Set, Op_Add, Op_Mov, Op_Rnd,
              Op_Check, Op_Len,

              Op_JNZ, Op_JZ,
              Op_JERR, Op_JNERR,
              Op_JNG, Op_JG,
              Op_JNL, Op_JL,
              Op_JEQ, Op_JNE,
              Op_JMP,
              };

class Creature;
class Cell;

enum OperandType { Ot_None = 0, Ot_Constant, Ot_ConstantVector, Ot_IntMemoryVector };

struct OpOptions {
    int64_t			baseCost;
    int64_t			lengthMul;
    OperandType     targetOpType;
};

class Instruction {
    static std::vector<OpOptions> opOptions;
public:
    static void prepare_instructions();
    OpOptions options();

    enum OpCode		code;
	enum Direction	dir;
	Coord			arg1;
	Coord			arg2;
	
    Instruction():code(Op_Nop), dir(Forward) {}
    Instruction(Instruction &org):code(org.code), dir(org.dir), arg1(org.arg1), arg2(org.arg2) {}
    Instruction(Instruction &&org):code(org.code), dir(org.dir), arg1(org.arg1), arg2(org.arg2) {}
	Instruction(enum Direction _dir, enum OpCode _code, Coord _arg1, Coord _arg2):
	code(_code), dir(_dir), arg1(_arg1), arg2(_arg2) {}
	Instruction(enum Direction _dir, enum OpCode _code, Coord _arg1): code(_code), dir(_dir), arg1(_arg1) {}
	Instruction(enum Direction _dir, enum OpCode _code): code(_code), dir(_dir) {}

    Instruction& operator=(const Instruction& src);

    std::string name(Creature *crt, Cell &cell);
    uint64_t cost(Creature *crt, bool condition);

};


#endif
