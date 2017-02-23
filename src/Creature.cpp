#include <iostream>
#include <ostream>
#include <cstdlib>

#include "World.h"

std::atomic<long int> Creature::CrIdCnt(0);

void Creature::print()
{
}

Creature::Creature(Coord entryPoint, int64_t _eng)
{
    Id = ++CrIdCnt;
    entry = entryPoint;
    ptr = entryPoint;
    flags = 0;
    energy = _eng;
    child_energy = 0;
    m_flag = false;
    lifetime = 0;

    Log::Not << "<<CRT " << Id << " [" << ptr.x << ":" << ptr.y << " E "<< "] was born.>>" << log4cpp::eol;

    counter1 = 0;
    counter2 = 0;
}

Creature::~Creature()
{
    World *theWorld = World::getInstance();

    Log::Not <<
        "<<CRT " << Id << " is died (length "<< fingerprint.size() << "). His life was " << lifetime << " instructions long. >>" << log4cpp::eol;

    for (unsigned long step:fingerprint) {
        Cell &cell = theWorld->getCell(step);
        //Log::Inf << "<<!!! CRT " << Id << " " << step%World::worldSize << ":"<< step/World::worldSize << ">>" << log4cpp::eol;
        cell.clear();
    }
    fingerprint.clear();
    doAbort();
}

void Creature::doAbort()
{
    World *theWorld = World::getInstance();

    if (embrion.size()>0) {
        Log::Inf << " CRT " << Id << " has " << embrion.size() << " embrional cells." << log4cpp::eol;
        for (unsigned long step:embrion) {
            Cell &cell = theWorld->getCell(step);
            if (cell.tailId == Id) {
                    //Log::Inf <<
                    //    "<<!!! CRT " << Id << " " << step%World::worldSize << ":"<< step/World::worldSize << ">>" << log4cpp::eol;
                cell.clear();
            }
        }
        embrion.clear();
    }
}

#define MUTATIONS_OFF 0

//
// Place of mutations
//
void Creature::applyPop(Coord &targetCoord, Cell &target, Instruction &value, const Direction src_dir)
{
    World *theWorld = World::getInstance();
    Direction tgt_dir = src_dir;

    // мутация-удаление
    do {
        // частота мутаций относительно количества выполненных инструкций
        if (std::rand()%MUTATIONS_LEVEL_DELETE != 0 || MUTATIONS_OFF || m_flag) {
            break;
        }

        // пока только пустышки
        if (value.code == Op_Nop) {

            m_flag = true;

            // корректируем указатель на цель, чтобы дальнейшее копирование выполнялось корректно
            Cell  &cell = theWorld->getCell(ptr);
            Coord &vect = internal_memory[cell.instruction.arg1.x];
            vect = vect.prev(src_dir);

            Coord to = ptr.add(vect);
            Log::Not << "CRT " << Id << " <<Op deleted in [" << to.x << ":" << to.y << "]" << log4cpp::eol;
            return;
        }

    } while (0);

    // мутация-смена направления выполнения кода
    do {
        if (std::rand()%MUTATIONS_LEVEL_DIRECTION != 0  || MUTATIONS_OFF || m_flag) {
            break;
        }
        switch (src_dir) {
        case Forward:
            tgt_dir = Right;
            break;
        case Right:
            tgt_dir = Back;
            break;
        case Back:
            tgt_dir = Left;
            break;
        case Left:
            tgt_dir = Forward;
            break;
        }
        Log::Not << "CRT " << Id << " Direction of [" << targetCoord.x << ":" << targetCoord.y
                 << "] changed from " << static_cast<int>(src_dir) << " to "
                 << static_cast<int>(tgt_dir) << log4cpp::eol;
        m_flag = true;
    }
    while (1);


    // копирование операции
    target.instruction = value;
    target.tailId = Id;
    target.executorId = 0;
    target.dir = tgt_dir;
    embrion.push_back(theWorld->cellIdx(targetCoord.x, targetCoord.y));
    OpOptions opt = value.options();

    // мутация-корректировка констант
    do {
        break;

        if (std::rand()%MUTATIONS_LEVEL_CONSTANT != 0  || MUTATIONS_OFF || m_flag) {
            break;
        }
        if (opt.sourceOpType == Ot_ConstantVector) {
            Coord delta;
            if (value.arg2.x) {
                delta.x = std::rand()%MUTATIONS_CONSTANT_MAXD;
            }
            if (value.arg2.y) {
                delta.y = std::rand()%MUTATIONS_CONSTANT_MAXD;
            }
            value.arg2 = value.arg2.add_raw(delta);
            m_flag = true;
        }
    }
    while (1);

    // мутация-добавление
    do {
        // частота мутаций относительно количества выполненных инструкций

        if (std::rand()%MUTATIONS_LEVEL_INSERT != 0 || MUTATIONS_OFF || m_flag) {
            break;
        }

        // insert one instruction after current
        m_flag = true;

        // корректируем указатель на цель, чтобы дальнейшее копирование выполнялось корректно
        Cell  &cell = theWorld->getCell(ptr);
        Coord &vect = internal_memory[cell.instruction.arg1.x];
        vect = vect.next(tgt_dir);

        // вставляем еще одну инструкцию
        //Coord to = ptr.add(vect);
        Coord to = targetCoord.next(tgt_dir);
        Cell &targetCell = theWorld->getCell(to);
        targetCell.instruction.code = Op_Nop;
        targetCell.tailId = Id;
        targetCell.executorId = 0;
        targetCell.dir = tgt_dir;
        embrion.push_back(theWorld->cellIdx(to.x, to.y));

        Log::Not << "CRT " << Id << " <<Nop inserted into [" << to.x << ":" << to.y << "]>>" << log4cpp::eol;

        return;

    } while (0);

}

void Creature::execInstruction()
{
    World *theWorld = World::getInstance();

    Cell        &cell = theWorld->getCell(ptr);
    Instruction &instruction = cell.instruction;
    bool        considerTheDistance = false;
    bool        simpleDirection = false;

    cell.executorId = Id;
    lifetime++;

    Log::Inf << "CRT " << Id << "\t"
             << ptr.x << ":" << ptr.y
             << " ST " << points.size()
             << "\tE " << energy
             << "\tFL "
             << ((flags&CRT_FLAG_ERR)?"E":"_")
             << ((flags&CRT_FLAG_Z)?"Z":"_")
             << ((flags&CRT_FLAG_GREATER)?"G":"_")
             << ((flags&CRT_FLAG_LESS)?"L":"_")
             << "\t";

    switch (instruction.code) {

    case Op_Nop: {
        Log::Inf << "NOP\t\t";
        simpleDirection = true;
        break;
    }

    case Op_Reset:{
        Log::Inf << "RESET\t\t";

        while (internal_stack.size()) {
            internal_stack.pop();
        }

        doAbort();

        if (m_flag) {
            Log::Inf << " <<Reset mutation data>>";
            m_flag = false;
        }

        child_energy = 0;
        flags = 0;
        simpleDirection = true;
        break;
    }

    case Op_Start:{
        simpleDirection = true;
        Log::Inf << "START\t\t" << instruction.arg1.x << ":" << instruction.arg1.y;

        Coord vect = internal_memory[instruction.arg1.x];
        int     idx = instruction.arg2.x;

        if (points.size()<idx+1) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " failed (call stack size " << points.size() << ", idx " << idx << ")";
            break;
        }

        Coord base = points[idx];
        Coord childEntry = base.add(vect);

        Log::Inf << " [" << childEntry.x << ":" << childEntry.y << "]";

        if (entry == childEntry || child_energy <= 0) {
            Log::Inf <<  " failed";
            child_energy = 0;
            doAbort();
        }
        else {
            // рожаем
            Creature *newcomer = new Creature(childEntry, child_energy);
            child_energy = 0;

            for (unsigned long step:embrion) {
                newcomer->fingerprint.push_back(step);
            }
            theWorld->startCreature(newcomer);

            Log::Inf << " " << newcomer->fingerprint.size();
            considerTheDistance = true;
        }
        embrion.clear();
        break;
    }

    case Op_Push:{
        simpleDirection = true;
        Coord   vect = internal_memory[instruction.arg1.x];
        int     idx = instruction.arg2.x;
        Log::Inf << "PUSH\t\t <- (" << instruction.arg1.x << ") " << vect.x << ":" << vect.y << "";

        if (points.size()<idx+1) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " failed (call stack size " << points.size() << ", idx " << idx << ")";
            break;
        }
        Coord base = points[idx];
        Coord from = base.add(vect);

        Log::Inf << " [" << from.x << ":" << from.y << "]";
        Cell &source = theWorld->getCell(from);
        internal_stack.push(source);
        internal_memory[instruction.arg1.x].inc(source.dir);
        considerTheDistance = true;
        break;
    }
    case Op_Pop:{
        Coord   vect = internal_memory[instruction.arg1.x];
        int     idx = instruction.arg2.x;
        Log::Inf << "POP\t\t -> (" << instruction.arg1.x << ") " << vect.x << ":" << vect.y << "";

        if (points.size()<idx+1) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " failed (call stack size " << points.size() << ", idx " << idx << ")";
            break;
        }
        Coord base = points[idx];
        Coord to = base.add(vect);

        simpleDirection = true;
        considerTheDistance = true;

        if (!internal_stack.size()) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " [" << to.x << ":" << to.y << "] failed (stack is empty)";
            break;
        }

        Cell    value = internal_stack.top();
        internal_stack.pop();
        Cell &target = theWorld->getCell(to);

        // ToDo: тут еще думать и думать над условиями возможности/невозможности
        if (target.instruction.code != Op_None) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " [" << to.x << ":" << to.y << "] failed (" << target.instruction.code << ","<< target.executorId << ","<< target.tailId <<  ")";
            break;
        }

        // собираем всю энергию поля и обнуляем
        child_energy += target.takeEnergy();

        flags &= (~CRT_FLAG_ERR);

        // записываем туда что-то
        applyPop(to, target, value.instruction, cell.dir);
        internal_memory[instruction.arg1.x].inc(target.dir);

        Log::Inf << " [" << to.x << ":" << to.y << "]" << static_cast<int>(target.dir)
                 << " <- " << value.instruction.name(this, target);

        break;
    }

    case Op_GetE:{
        Coord   vect = internal_memory[instruction.arg2.x];
        Coord   from = ptr.add(vect);
        Cell    &source = theWorld->getCell(from);

        Log::Inf << "GETE\t\t (" << instruction.arg1.x << ") <- ("<< instruction.arg2.x << ") "
                 << vect.x << ":" << vect.y << " [" << from.x << ":" << from.y << "] = " << source.energy;

        simpleDirection = true;
        considerTheDistance = true;

        if (source.instruction.code != Op_None) {
            flags |= CRT_FLAG_ERR;
            internal_memory[instruction.arg1.x].x =  0;
            internal_memory[instruction.arg1.x].y =  0;
            Log::Inf << " failed (" << source.instruction.code << ","<< source.executorId << ","<< source.tailId <<  ")";
        }
        else {
            internal_memory[instruction.arg1.x].x =  source.energy;
            internal_memory[instruction.arg1.x].y =  source.dir;
        }
        break;
    }

    case Op_SetE:{
        Coord   vect = internal_memory[instruction.arg1.x];
        Coord   to = ptr.add(vect);
        int64_t gift = internal_memory[instruction.arg2.x].x;
        Log::Inf << "SETE\t\t <-(" << instruction.arg1.x << ") " << vect.x << ":" << vect.y << "";
        Log::Inf << " [" << to.x << ":" << to.y << "] <- (" << instruction.arg2.x << ") = "  << gift;
        Cell &target = theWorld->getCell(to);

        int64_t cost = instruction.cost(cell, *this, considerTheDistance);
        considerTheDistance = true;
        simpleDirection = true;

        if (target.instruction.code != Op_None) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " failed (" << target.instruction.code << ","<< target.executorId << ","<< target.tailId <<  ")";
            break;
        }

        if (cost+gift>energy) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << " failed (have not enought energy)";
            break;
        }

        flags &= (~CRT_FLAG_ERR);
        target.energy += gift;
        energy -= gift;

        break;
    }

    case Op_CmpTop: {
        Log::Inf << "CMPTOP\t\t " << instruction.arg1.x;
        Cell    value = internal_stack.top();
        if (value.energy < instruction.arg1.x) {
            flags |= CRT_FLAG_LESS;
            flags &= (~CRT_FLAG_GREATER);
        }
        else if (value.energy > instruction.arg1.x) {
            flags |= CRT_FLAG_GREATER;
            flags &= (~CRT_FLAG_LESS);
        }
        else {
            flags &= (~CRT_FLAG_GREATER);
            flags &= (~CRT_FLAG_LESS);
        }
        simpleDirection = true;
        break;
    }

    case Op_Set:{
        int int_addr = instruction.arg1.x;
        Log::Inf << "SET\t\t("  << int_addr << ") <- " << instruction.arg2.x << ":" << instruction.arg2.y;
        internal_memory[int_addr] = instruction.arg2;
        simpleDirection = true;
        break;
    }

    case Op_Add:{
        int int_addr = instruction.arg1.x;
        Log::Inf << "ADD\t\t(" << int_addr << ") += " << instruction.arg2.x << ":" << instruction.arg2.y;
        internal_memory[int_addr] = internal_memory[int_addr].add_raw(instruction.arg2);
        setFlags(internal_memory[int_addr].x);
        simpleDirection = true;
        break;
    }

    case Op_Cmp:{
        int int_addr = instruction.arg1.x;
        int32_t l1 = internal_memory[int_addr].length();
        int32_t l2 = instruction.arg2.length();
        Log::Inf << "CMP\t\t(" << int_addr << ") = " << l1 << " with "<< l2;
        setFlags(l1-l2);
        simpleDirection = true;
        break;
    }

    case Op_Mov:{
        int int_addr1 = instruction.arg1.x;
        int int_addr2 = instruction.arg2.x;
        Log::Inf << "MOV\t\t(" << int_addr1 << ") <- (" <<  int_addr2 << ")";
        internal_memory[instruction.arg1.x] = internal_memory[instruction.arg2.x];
        simpleDirection = true;
        break;
    }

    case Op_AddReg:{
        int int_addr1 = instruction.arg1.x;
        int int_addr2 = instruction.arg2.x;
        Coord sum;
        sum.x = internal_memory[instruction.arg1.x].x + internal_memory[instruction.arg2.x].x;
        sum.y = internal_memory[instruction.arg1.x].y + internal_memory[instruction.arg2.x].y;
        internal_memory[instruction.arg1.x] = sum;
        Log::Inf << "ADD\t\t(" << int_addr1 << ") <- (" <<  int_addr2 << ") = " << sum.x << ":" << sum.y;
        setFlags(internal_memory[int_addr1].x);
        simpleDirection = true;
        break;
    }

    case Op_Rnd:{
        int int_addr = instruction.arg1.x;
        Log::Inf << "RND\t\t(" << int_addr << ") += " << instruction.arg2.x << ":" << instruction.arg2.y;
        Coord cRnd(0,0);
        if (instruction.arg2.x) cRnd.x = std::rand()%instruction.arg2.x;
        if (instruction.arg2.y) cRnd.y = std::rand()%instruction.arg2.y;
        internal_memory[int_addr] = cRnd;
        simpleDirection = true;
        break;
    }

    case Op_Next:{
        int  int_addr = instruction.arg1.x;
        int  count = instruction.arg2.x;
        Coord tptr = ptr;
        while (count) {
            Cell &tcell = theWorld->getCell(tptr);
            internal_memory[int_addr].inc(tcell.dir);
            tptr.inc(tcell.dir);
            count --;
        }
        Log::Inf << "NEXT\t\t(" << int_addr << ") " << instruction.arg2.x
                 << " <- " << internal_memory[int_addr].x << ":" << internal_memory[int_addr].y;
        simpleDirection = true;
        break;
    }

    case Op_Prev:{
        int  int_addr = instruction.arg1.x;
        int  count = instruction.arg2.x;
        Coord tptr = ptr;
        while (count) {
            Cell &tcell = theWorld->getCell(tptr);
            internal_memory[int_addr].dec(tcell.dir);
            tptr.dec(tcell.dir);
            count --;
        }
        Log::Inf << "PREV\t\t(" << int_addr << ") " << instruction.arg2.x
                 << " <- " << internal_memory[int_addr].x << ":" << internal_memory[int_addr].y;
        simpleDirection = true;
        break;
    }

    case Op_Len:{
        int int_addr = instruction.arg1.x;
        internal_memory[int_addr].x = fingerprint.size();
        internal_memory[int_addr].y = fingerprint.size();
        Log::Inf << "LEN\t\t(" << int_addr << ") <- " << internal_memory[int_addr].x << ":" << internal_memory[int_addr].y;
        simpleDirection = true;
        break;
    }

    case Op_Check:{
        Log::Inf << "CHECK\t\t(" << instruction.arg1.x << ") " << internal_memory[instruction.arg1.x].x;
        Creature::setFlags(internal_memory[instruction.arg1.x].x);
        simpleDirection = true;
        break;
    }

    case Op_Begin: {
        points.push(ptr);
        Log::Inf << "BEGIN\t";
        simpleDirection = true;
        break;
    }

    case Op_End: {
        Log::Inf << "END\t\t";
        simpleDirection = !jumpToBegin();
        break;
    }

    case Op_BreakOnErr: {
        Log::Inf << "BREAK ON ERR\t";
        if (flags & CRT_FLAG_ERR) {
            jumpToEnd();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_ContinueOnErr: {
        if (flags & CRT_FLAG_ERR) {
            Log::Inf << "CONTINUE ON ERR*\t";
            simpleDirection = !jumpToBegin();
        }
        else {
            Log::Inf << "CONTINUE ON ERR\t";
            simpleDirection = true;
        }
        break;
    }

    case Op_BreakOnG: {
        Log::Inf << "BREAK ON G\t";
        if (flags & CRT_FLAG_GREATER) {
            jumpToEnd();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_ContinueOnG: {
        Log::Inf << "CONTINUE ON G\t";
        if (flags & CRT_FLAG_GREATER) {
            simpleDirection = !jumpToBegin();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_BreakOnL: {
        Log::Inf << "BREAK ON L\t";
        if (flags & CRT_FLAG_LESS) {
            jumpToEnd();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_ContinueOnL: {
        Log::Inf << "CONTINUE ON L\t";
        if (flags & CRT_FLAG_LESS) {
            simpleDirection = !jumpToBegin();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_BreakOnZ: {
        if (flags & CRT_FLAG_Z) {
            Log::Inf << "BREAK ON Z*\t";
            jumpToEnd();
        }
        else {
            Log::Inf << "BREAK ON Z\t";
            simpleDirection = true;
        }
        break;
    }

    case Op_ContinueOnZ: {
        Log::Inf << "CONTINUE ON Z\t";
        if (flags & CRT_FLAG_Z) {
            simpleDirection = !jumpToBegin();
        }
        else {
            simpleDirection = true;
        }
        break;
    }

    case Op_None: {
        // freeze
        Log::Inf << "NONE\t\t" << instruction.code;
        break;
    }
    }

    Log::Inf << log4cpp::eol;

    energy -= instruction.cost(cell, *this, considerTheDistance);

    if (simpleDirection) {
        ptr = ptr.next(cell.dir);
    }
}

bool Creature::jumpToBegin()
{
    if (!points.size()) {
        flags |= CRT_FLAG_ERR;
        Log::Inf << "Failed";
        return false;
    }
    else {
        Coord nPtr = points.top();
        points.pop();
        ptr = nPtr;
        Log::Inf << "[" << ptr.x << ":" << ptr.y << "]";
        return true;
    }
}

bool Creature::jumpToEnd()
{
    Instruction ins;
    Direction   dir;
    //Log::Inf << log4cpp::eol;

    do {
        ins = World::getInstance()->getCell(ptr).instruction;
        dir = World::getInstance()->getCell(ptr).dir;
        //Log::Inf << "CRT " << Id << " [" << ptr.x << ":" << ptr.y << "] " << static_cast<int>(dir);
        //Log::Inf << log4cpp::eol;
        ptr = ptr.next(dir);
    }
    while (!ins.is(Op_End));

    if (points.size()) {
        points.pop();
    }
    return true;
}

void Creature::moveBy(int32_t steps)
{
    if (steps<0) {
        int32_t cnt = -steps;
        while (cnt--) {
            Cell &tcell = World::getInstance()->getCell(ptr);
            ptr.dec(tcell.dir);
        }
    }
    else  if (steps>0) {
        int32_t cnt = steps;
        while (cnt--) {
            Cell &tcell = World::getInstance()->getCell(ptr);
            ptr.inc(tcell.dir);
        }
    }
}

void Creature::setFlags(int32_t value)
{
    if (value > 0) {
        flags &= (~CRT_FLAG_Z);
        flags |= CRT_FLAG_GREATER;
        flags &= (~CRT_FLAG_LESS);
    }
    else if (value < 0) {
        flags &= (~CRT_FLAG_Z);
        flags |= CRT_FLAG_LESS;
        flags &= (~CRT_FLAG_GREATER);
    }
    else {
        flags |= CRT_FLAG_Z;
        flags &= (~CRT_FLAG_LESS);
        flags &= (~CRT_FLAG_GREATER);
    }
}

