#include "World.h"

std::vector<OpOptions> Instruction::opOptions;

void Instruction::prepare_instructions()
{
    Instruction::opOptions.resize(static_cast<int>(Op_Last));

    Instruction::opOptions[Op_None]  =  {ENERGY_COST_FAIL,           0, Ot_None,              Ot_None};
    Instruction::opOptions[Op_Nop]   =  {1,                          0, Ot_None,              Ot_None};
    Instruction::opOptions[Op_Start] =  {200,                        1, Ot_IntMemoryPtr,      Ot_None};
    Instruction::opOptions[Op_Reset] =  {10,                         0, Ot_None,              Ot_None};

    // вектор в ячейке внутренней памяти, адрес которой задается операндом
    Instruction::opOptions[Op_Push ] =  {ENERGY_COST_SENSOR,         1, Ot_IntMemoryPtr,      Ot_ConstantVector};
    Instruction::opOptions[Op_Pop  ] =  {ENERGY_COST_MODIFICATOR,    2, Ot_IntMemoryPtr,      Ot_ConstantVector};

    Instruction::opOptions[Op_GetE  ] =  {ENERGY_COST_SENSOR,        1, Ot_IntMemory,         Ot_IntMemoryPtr};
    Instruction::opOptions[Op_SetE  ] =  {ENERGY_COST_MODIFICATOR,   1, Ot_IntMemoryPtr,      Ot_IntMemory};

    // операнд - константа не связанная с внешним миром
    Instruction::opOptions[Op_CmpTop] =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_None};
    Instruction::opOptions[Op_Set]    =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_ConstantVector};
    Instruction::opOptions[Op_Add]    =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_ConstantVector};
    Instruction::opOptions[Op_Rnd]    =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_ConstantVector};
    Instruction::opOptions[Op_Mov]    =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_IntMemory};
    Instruction::opOptions[Op_AddReg] =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_IntMemory};
    Instruction::opOptions[Op_Len]    =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_None};
    Instruction::opOptions[Op_Check]  =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_None};
    Instruction::opOptions[Op_Cmp  ]  =  {ENERGY_COST_INTERNAL,      0, Ot_IntMemory,         Ot_ConstantVector};
    Instruction::opOptions[Op_Next]  =  {ENERGY_COST_INTERNAL,       0, Ot_IntMemory,         Ot_ConstantVector};
    Instruction::opOptions[Op_Prev]  =  {ENERGY_COST_INTERNAL,       0, Ot_IntMemory,         Ot_ConstantVector};

    // операнд - константный вектор во внешний мир относительно текущей ячейки
    Instruction::opOptions[Op_Begin]         =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_BreakOnErr]    =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_ContinueOnErr] =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_BreakOnZ]      =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_ContinueOnZ]   =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_BreakOnG]      =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_ContinueOnG]   =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_BreakOnL]      =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_ContinueOnL]   =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};
    Instruction::opOptions[Op_End]           =  {ENERGY_COST_INTERNAL,       1, Ot_None,   Ot_None};


    Log::Not <<  "Prepared " << Instruction::opOptions.size() << " types of instructions\n";
}

OpOptions Instruction::options()
{
    return opOptions[code%Instruction::opOptions.size()];
}

uint64_t Instruction::cost(Cell &cell, Creature &crt, bool condition)
{
    //World *theWorld = World::getInstance();

    OpOptions opt = options();
    uint64_t ee = opt.baseCost;

    // constant address
    if (opt.targetOpType == Ot_ConstantVector && condition) {
        Coord vect = cell.instruction.arg1;
        ee += opt.lengthMul*vect.length();
    }
    // address from internal memory
    else {
        if (opt.targetOpType == Ot_IntMemoryPtr && condition) {
            Coord vect = crt.internal_memory[cell.instruction.arg1.x];
            ee += opt.lengthMul*vect.length();
        }
        if (opt.sourceOpType == Ot_IntMemoryPtr && condition) {
            Coord vect = crt.internal_memory[cell.instruction.arg2.x];
            ee += opt.lengthMul*vect.length();
        }
    }

    return ee;
}

Instruction& Instruction::operator=(const Instruction& src)
{
	if (this == &src) {
		return *this;
	}
	code = src.code;
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
        case Op_GetE:{
            op_name =  "GETE (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") <- ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg2.y));
            break;
        }
        case Op_SetE:{
            op_name =  "SETE (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg1.y));
            op_name.append(") <- ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
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
        case Op_Cmp:{
            op_name =  "CMP (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") with ");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(":");
            op_name.append(std::to_string(cell.instruction.arg2.y));
            break;
        }
        case Op_AddReg:{
            op_name =  "ADD (";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            op_name.append(") += (");
            op_name.append(std::to_string(cell.instruction.arg2.x));
            op_name.append(")");
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
        case Op_Next:{
            op_name =  "NEXT ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
            break;
        }
        case Op_Prev:{
            op_name =  "PREV ";
            op_name.append(std::to_string(cell.instruction.arg1.x));
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

        case Op_Begin: {
            op_name =  "BEGIN";
            break;
        }
        case Op_BreakOnErr: {
            op_name =  "BREAKONERR";
            break;
        }
        case Op_ContinueOnErr: {
            op_name =  "CONTINUEONERR";
            break;
        }
        case Op_BreakOnZ: {
            op_name =  "BREAKONZ";
            break;
        }
        case Op_ContinueOnZ: {
            op_name =  "CONTINUEONZ";
            break;
        }
        case Op_BreakOnG: {
            op_name =  "BREAKONG";
            break;
        }
        case Op_ContinueOnG: {
            op_name =  "CONTINUEONG";
            break;
        }
        case Op_BreakOnL: {
            op_name =  "BREAKONL";
            break;
        }
        case Op_ContinueOnL: {
            op_name =  "CONTINUEONL";
            break;
        }
        case Op_End: {
            op_name =  "END";
            break;
        }

    }
    return op_name;
}

