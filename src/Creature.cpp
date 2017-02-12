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

//
// Place of mutations
//
void Creature::applyPop(Coord &targetCoord, Cell &target, Instruction &value)
{
    World *theWorld = World::getInstance();

    // мутация-удаление
    if (!m_flag) {

        do {
            //counter1++;
            //if (counter1%18 != 0)
            //    break;

            // частота мутаций относительно количества выполненных инструкций
            if (std::rand()%MUTATIONS_LEVEL_DELETE != 0) {
                break;
            }

            // пока только пустышки
            if (value.code == Op_Nop) {

                m_flag = true;

                // корректируем указатель на цель, чтобы дальнейшее копирование выполнялось корректно
                Cell  &cell = theWorld->getCell(ptr);
                Coord &vect = internal_memory[cell.instruction.arg1.x];
                vect = vect.prev(value.dir);

                switch (value.dir) {
                    case Forward: {
                        m_adjust.x = -1;
                        m_adjust.y = 0;
                        break;
                    }
                    case Back:{
                        m_adjust.x = 1;
                        m_adjust.y = 0;
                        break;
                    }
                    case Left:{
                        m_adjust.x = 0;
                        m_adjust.y = 1;
                        break;
                    }
                    case Right:{
                        m_adjust.x = 0;
                        m_adjust.y = -1;
                        break;
                    }
                }

                m_pos = ptr.add(vect);
                Log::Not
                    << "CRT " << Id << " <<Op deleted in [" << m_pos.x << ":" << m_pos.y << "]"
                    << " adjust " << m_adjust.x << ":" << m_adjust.y << ">>" << log4cpp::eol;

                return;
            }

        } while (0);
    }

    // копирование операции
    target.instruction = value;
    target.tailId = Id;
    target.executorId = 0;
    embrion.push_back(theWorld->cellIdx(targetCoord.x, targetCoord.y));


    // мутация-добавление
    if (!m_flag) {
        do {

            //counter2++;
            //*p_log_root << log4cpp::Priority::INFO << "CRT " << Id << " COUNTER " << counter2;
            //if (counter2%6 != 0)
            //    break;

            // частота мутаций относительно количества выполненных инструкций
            if (std::rand()%MUTATIONS_LEVEL_INSERT != 0) {
                break;
            }

            // insert one instruction after current
            m_flag = true;

            // корректируем указатель на цель, чтобы дальнейшее копирование выполнялось корректно
            Cell  &cell = theWorld->getCell(ptr);
            Coord &vect = internal_memory[cell.instruction.arg1.x];
            vect = vect.next(value.dir);

            switch (value.dir) {
                case Forward: {
                    m_adjust.x = 1;
                    m_adjust.y = 0;
                    break;
                }
                case Back:{
                    m_adjust.x = -1;
                    m_adjust.y = 0;
                    break;
                }
                case Left:{
                    m_adjust.x = 0;
                    m_adjust.y = -1;
                    break;
                }
                case Right:{
                    m_adjust.x = 0;
                    m_adjust.y = 1;
                    break;
                }
            }

            Coord to = ptr.add(vect);
            Cell &targetCell = theWorld->getCell(to);
            targetCell.instruction.code = Op_Nop;
            targetCell.instruction.dir = target.instruction.dir;
            targetCell.tailId = Id;

            embrion.push_back(theWorld->cellIdx(to.x, to.y));

            m_pos = to;
            Log::Not << "CRT " << Id << " <<Nop inserted into [" << m_pos.x << ":" << m_pos.y << "]>>" << log4cpp::eol;

            return;

        } while (0);
    }

    if (m_flag) {

        // новое копирование (тип операции - плохой признак, но хоть что-то)
        // обнуляем данные мутации
        if (value.code == Op_Reset) {
            Log::Not << " <<Reset mutation data>>" << log4cpp::eol;
            m_flag = false;
            m_pos.x = 0;
            m_pos.y = 0;
            m_adjust.x = 0;
            m_adjust.y = 0;
        }

        // правки касаются только переходов
        OpOptions opt = value.options();
        if (opt.targetOpType != Ot_ConstantVector) {
            return;
        }

        // вектор указывающий на точку изменения
        Coord delta = m_pos.add_raw(-targetCoord.x, -targetCoord.y);

        Log::Not << "CRT " << Id << " <<Found jump vector " << value.arg1.x << ":" << value.arg1.y << ">>" << log4cpp::eol;

        // если вектор этой операции указывает на точку "через голову" сохраненной позиции мутации
        // хотя бы по одному из имерений - вносим в него поправку
        if ((delta.x<0 && value.arg1.x<0 && value.arg1.x<=delta.x) ||
            (delta.x>0 && value.arg1.x>0 && value.arg1.x>=delta.x) ||
            (delta.y<0 && value.arg1.y<0 && value.arg1.y<=delta.y) ||
            (delta.y>0 && value.arg1.y>0 && value.arg1.y>=delta.y))
        {

            Coord newValue = value.arg1.add_raw(-m_adjust.x,-m_adjust.y);

            Log::Not
                << "CRT " << Id << " <<Jump with vector " << value.arg1.x << ":" << value.arg1.y
                << " ajusted with " << m_adjust.x << ":" << m_adjust.y << " into " << newValue.x << ":" << newValue.y << ">>" << log4cpp::eol;

            value.arg1 = newValue;
            target.instruction = value;
        }
    }
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

    Log::Inf << "CRT " << Id << "\t" << ptr.x << ":" << ptr.y << "\tE " << energy << "\tFL " << flags << "\t";

    switch (instruction.code) {

    case Op_Nop: {
        Log::Inf << "NOP\t";
        simpleDirection = true;
        break;
    }

    case Op_Reset:{
        Log::Inf << "RESET\t";

        while (internal_stack.size()) {
            internal_stack.pop();
        }
        while (points.size()) {
            points.pop();
        }

        doAbort();

        if (m_flag) {
            Log::Inf << " <<Reset mutation data>>";
            m_flag = false;
            m_pos.x = 0;
            m_pos.y = 0;
            m_adjust.x = 0;
            m_adjust.y = 0;
        }

        child_energy = 0;
        flags = 0;
        simpleDirection = true;
        break;
    }
    case Op_Start:{
        Log::Inf << "START\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        Coord vect = internal_memory[instruction.arg1.x];
        Coord childEntry = ptr.add(vect);
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
        simpleDirection = true;
        break;
    }

    case Op_Push:{
        Coord   vect = internal_memory[instruction.arg1.x];
        Coord   from = ptr.add(vect);
        Log::Inf << "PUSH\t <-(" << instruction.arg1.x << ") " << vect.x << ":" << vect.y << "";
        Log::Inf << " [" << from.x << ":" << from.y << "]";// <- " << source.instruction.name(crt, source);
        Cell &source = theWorld->getCell(from);
        internal_stack.push(source);
        considerTheDistance = true;
        simpleDirection = true;
        break;
    }
    case Op_Pop:{
        Coord   vect = internal_memory[instruction.arg1.x];
        Log::Inf << "POP\t ->(" << instruction.arg1.x << ") " << vect.x << ":" << vect.y << "";
        Coord   to = ptr.add(vect);

        simpleDirection = true;

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
        considerTheDistance = true;

        // записываем туда что-то
        applyPop(to, target, value.instruction);

        Log::Inf << " [" << to.x << ":" << to.y << "] <- " << value.instruction.name(this, target);

        break;
    }
    case Op_CmpTop: {
        Log::Inf << "CMPTOP\t " << instruction.arg1.x;
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
        Log::Inf << "SET\t("  << int_addr << ") <- " << instruction.arg2.x << ":" << instruction.arg2.y;
        internal_memory[int_addr] = instruction.arg2;
        simpleDirection = true;
        break;
    }

    case Op_Add:{
        int int_addr = instruction.arg1.x;
        Log::Inf << "ADD\t(" << int_addr << ") += " << instruction.arg2.x << ":" << instruction.arg2.y;
        internal_memory[int_addr] = internal_memory[int_addr].add_raw(instruction.arg2);
        simpleDirection = true;
        break;
    }

    case Op_Rnd:{
        int int_addr = instruction.arg1.x;
        Log::Inf << "RND\t(" << int_addr << ") += " << instruction.arg2.x << ":" << instruction.arg2.y;
        Coord cRnd(0,0);
        if (instruction.arg2.x) cRnd.x = std::rand()%instruction.arg2.x;
        if (instruction.arg2.y) cRnd.y = std::rand()%instruction.arg2.y;
        internal_memory[int_addr] = cRnd;
        simpleDirection = true;
        break;
    }

    case Op_Len:{
        int int_addr = instruction.arg1.x;
        internal_memory[int_addr].x = fingerprint.size();
        internal_memory[int_addr].y = fingerprint.size();
        Log::Inf << "LEN\t(" << int_addr << ") <- " << internal_memory[int_addr].x << ":" << internal_memory[int_addr].y;
        simpleDirection = true;
        break;
    }

    case Op_Mark:{
        points.push(ptr);
        Log::Inf << "MARK\t " << ptr.x << ":" << ptr.y;
        simpleDirection = true;
        break;
    }

    case Op_Distance:{
        int int_addr = instruction.arg1.x;

        if (!points.size()) {
            flags |= CRT_FLAG_ERR;
            Log::Inf << "DISTANCE\tFailed";
        }
        else {
            Coord storedPtr = points.top();
            points.pop();
            flags &= (~CRT_FLAG_ERR);
            Coord vect = storedPtr.add_raw(-ptr.x, -ptr.y);
            internal_memory[int_addr] = internal_memory[int_addr].add_raw(vect.x, vect.y);
            Log::Inf << "DISTANCE (" << int_addr << ") <- "
                << vect.x << ":" << vect.y << " " << internal_memory[int_addr].x << ":" << internal_memory[int_addr].y;
        }

        simpleDirection = true;
        break;
    }

    case Op_Mov:{
        int int_addr1 = instruction.arg1.x;
        int int_addr2 = instruction.arg2.x;
        Log::Inf << "MOV\t(" << int_addr1 << ") <- (" <<  int_addr2 << ")";
        internal_memory[instruction.arg1.x] = internal_memory[instruction.arg2.x];
        simpleDirection = true;
        break;
    }
    case Op_Check:{
        Log::Inf << "CHECK\t(" << instruction.arg1.x << ") " << internal_memory[instruction.arg1.x].x;
        if (internal_memory[instruction.arg1.x].x) {
            flags &= (~CRT_FLAG_Z);
        }
        else {
            flags |= CRT_FLAG_Z;
        }
        if (internal_memory[instruction.arg1.x].x > 0) {
            flags |= CRT_FLAG_GREATER;
            flags &= (~CRT_FLAG_LESS);
        }
        else if (internal_memory[instruction.arg1.x].x < 0) {
            flags |= CRT_FLAG_LESS;
            flags &= (~CRT_FLAG_GREATER);
        }
        else {
            flags &= (~CRT_FLAG_LESS);
            flags &= (~CRT_FLAG_GREATER);
        }
        simpleDirection = true;
        break;
    }

    case Op_JERR: {
        if (flags & CRT_FLAG_ERR) {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << "JERR*\t" << instruction.arg1.x << ":" << instruction.arg1.y;
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            ptr = target;
            considerTheDistance = true;
        }
        else {
            Log::Inf << "JERR\t" << instruction.arg1.x << ":" << instruction.arg1.y;
            simpleDirection = true;
        }
        break;
    }

    case Op_JNERR: {
        Log::Inf << "JNERR\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_ERR) {
            simpleDirection = true;
        }
        else {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            considerTheDistance = true;
            ptr = target;
        }
        break;
    }

    case Op_JNZ: {
        Log::Inf << "JNZ\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_Z) {
            simpleDirection = true;
        }
        else {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            ptr = target;
            considerTheDistance = true;
        }
        break;
    }
    case Op_JZ: {
        Log::Inf << "JZ\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_Z) {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            considerTheDistance = true;
            ptr = target;
        }
        else {
            simpleDirection = true;
        }
        break;
    }
    case Op_JNG: {
        Log::Inf << "JNG\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_GREATER) {
            simpleDirection = true;
        }
        else {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            ptr = target;
            considerTheDistance = true;
        }
        break;
    }
    case Op_JG: {
        Log::Inf << "JG\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_GREATER) {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            considerTheDistance = true;
            ptr = target;
        }
        else {
            simpleDirection = true;
        }
        break;
    }
    case Op_JNL: {
        Log::Inf << "JNL\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_LESS) {
            simpleDirection = true;
        }
        else {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            ptr = target;
            considerTheDistance = true;
        }
        break;
    }
    case Op_JL: {
        Log::Inf << "JL\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & CRT_FLAG_LESS) {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            considerTheDistance = true;
            ptr = target;
        }
        else {
            simpleDirection = true;
        }
        break;
    }
    case Op_JEQ: {
        Log::Inf << "JEQ\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & (CRT_FLAG_LESS | CRT_FLAG_GREATER)) {
            simpleDirection = true;
        }
        else {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            ptr = target;
            considerTheDistance = true;
        }
        break;
    }
    case Op_JNE: {
        Log::Inf << "JNE\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        if (flags & (CRT_FLAG_LESS | CRT_FLAG_GREATER)) {
            Coord target = ptr.add(instruction.arg1);
            Log::Inf << " [" << target.x << ":" << target.y << "]";
            considerTheDistance = true;
            ptr = target;
        }
        else {
            simpleDirection = true;
        }
        break;
    }
    case Op_JMP: {
        Coord target = ptr.add(instruction.arg1);
        Log::Inf << "JMP\t" << instruction.arg1.x << ":" << instruction.arg1.y;
        Log::Inf << " [" << target.x << ":" << target.y << "]";
        considerTheDistance = true;
        ptr = target;
        break;
    }
    case Op_None: {
        // freeze
        Log::Inf << "NONE " << instruction.code;
        break;
    }
    }

    Log::Inf << log4cpp::eol;

    energy -= instruction.cost(this, considerTheDistance);

    if (simpleDirection) {
        ptr = ptr.next(instruction.dir);
    }

}

