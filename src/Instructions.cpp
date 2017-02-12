#include "World.h"

std::vector<OpOptions> Instruction::opOptions;

void Instruction::prepare_instructions()
{
    Instruction::opOptions.resize(26);

    Instruction::opOptions[Op_None]  =  {ENERGY_COST_FAIL,          0, Ot_None};
    Instruction::opOptions[Op_Nop]   =  {1,                         0, Ot_None};
    Instruction::opOptions[Op_Start] =  {200,                       1, Ot_IntMemoryVector};
    Instruction::opOptions[Op_Reset] =  {10,                        0, Ot_None};

    // вектор в ячейке внутренней памяти, адрес которой задается операндом
    Instruction::opOptions[Op_Push ] =  {ENERGY_COST_SENSOR,        1, Ot_IntMemoryVector};
    Instruction::opOptions[Op_Pop  ] =  {ENERGY_COST_MODIFICATOR,   2, Ot_IntMemoryVector};

    // операнд - константа не связанная с внешним миром
    Instruction::opOptions[Op_CmpTop] =  {ENERGY_COST_INTERNAL,      0, Ot_Constant};
    Instruction::opOptions[Op_Set]    =  {ENERGY_COST_INTERNAL,      0, Ot_Constant};
    Instruction::opOptions[Op_Add]    =  {ENERGY_COST_INTERNAL,      0, Ot_Constant};
    Instruction::opOptions[Op_Rnd]    =  {ENERGY_COST_INTERNAL,      0, Ot_Constant};
    Instruction::opOptions[Op_Mov]    =  {ENERGY_COST_INTERNAL,      0, Ot_None};
    Instruction::opOptions[Op_Len]    =  {ENERGY_COST_INTERNAL,      0, Ot_None};
    Instruction::opOptions[Op_Check]  =  {ENERGY_COST_INTERNAL,      0, Ot_None};

    Instruction::opOptions[Op_Mark]     =  {ENERGY_COST_INTERNAL,      0, Ot_None};
    Instruction::opOptions[Op_Distance] =  {ENERGY_COST_INTERNAL,      0, Ot_None};

    // операнд - константный вектор во внешний мир относительно текущей ячейки
    Instruction::opOptions[Op_JNZ]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JZ]    =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JMP]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JERR]  =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JNERR] =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JNG]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JG]    =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JNL]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JL]    =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JEQ]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};
    Instruction::opOptions[Op_JNE]   =  {ENERGY_COST_INTERNAL,      1, Ot_ConstantVector};


    Log::Not <<  "Prepared " << Instruction::opOptions.size() << " types of instructions\n";
}

OpOptions Instruction::options()
{
    return opOptions[code%Instruction::opOptions.size()];
}

uint64_t Instruction::cost(Creature *crt, bool condition)
{
    World *theWorld = World::getInstance();

    OpOptions opt = options();
    uint64_t ee = opt.baseCost;

    if (opt.targetOpType == Ot_ConstantVector && condition) {
        Cell &cell = theWorld->getCell(crt->ptr);
        Coord vect = cell.instruction.arg1;
        ee += opt.lengthMul*vect.length();
    }
    else if (opt.targetOpType == Ot_IntMemoryVector && condition) {
        Cell &cell = theWorld->getCell(crt->ptr);
        Coord vect = crt->internal_memory[cell.instruction.arg1.x];
        ee += opt.lengthMul*vect.length();
    }

    return ee;
}

Instruction& Instruction::operator=(const Instruction& src)
{
	if (this == &src) {
		return *this;
	}
	code = src.code;
	dir = src.dir;
	arg1 = src.arg1;
	arg2 = src.arg2;
	return *this;	
}

// ToDo:
// use it in main instruction loop for logging
//
std::string Instruction::name(Creature *crt, Cell &cell)
{
    std::string op_name = std::to_string(code);
    switch (code) {
        case Op_None: {
            op_name =  "NONE";
            break;
        }
        case Op_Nop: {
            op_name =  "NOP";
            break;
        }
        case Op_Reset: {
            op_name =  "RESET";
            break;
        }
        case Op_Start:{
            op_name =  "START (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            op_name.append(")");
            break;
        }
        case Op_Push:{
            op_name =  "PUSH ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_Pop:{
            op_name =  "POP ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_CmpTop:{
            op_name =  "CMPTOP ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            break;
        }
        case Op_Set:{
            op_name =  "SET (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") <- ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg2.y));
            break;
        }
        case Op_Add:{
            op_name =  "ADD (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") += ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg2.y));
            break;
        }
        case Op_Mov:{
            op_name =  "MOV (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") <- (");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(")");
            break;
        }
        case Op_Check:{
            op_name =  "CHECK ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            break;
        }
        case Op_JNZ: {
            op_name =  "JNZ ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JZ: {
            op_name =  "JZ ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JNL: {
            op_name =  "JNL ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JL: {
            op_name =  "JL ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JNG: {
            op_name =  "JNG ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JG: {
            op_name =  "JG ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JEQ: {
            op_name =  "JEQ ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JNE: {
            op_name =  "JNE ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JNERR: {
            op_name =  "JNERR ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JERR: {
            op_name =  "JERR ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_JMP: {
            op_name =  "JMP ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            break;
        }
        case Op_Rnd: {
            op_name =  "RND (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") <- ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg2.y));
            break;
        }
        case Op_Len: {
            op_name =  "LEN (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(")");
            break;
        }
        case Op_Mark: {
            op_name =  "MARK";
            break;
        }
        case Op_Distance: {
            op_name =  "DISTANCE (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(")");
            break;
        }
    }
    return op_name;
}

