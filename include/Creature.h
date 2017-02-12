#ifndef CREATURE
#define CREATURE

#include <stack>
#include <vector>
#include <atomic>

#include "Defs.h"
#include "Coord.h"
#include "Cell.h"

#define CRT_FLAG_Z          1
#define CRT_FLAG_ERR        2
#define CRT_FLAG_GREATER    4
#define CRT_FLAG_LESS       8

struct Step {
    Coord   pos;
    Instruction instruction;

    Step():pos(0,0), instruction() {}
    Step(Coord _pos, Instruction _ins):pos(_pos),instruction(_ins) {}
};

typedef std::vector<unsigned long> Fingerprint;

class Creature {
public:
    static std::atomic<long int>         CrIdCnt;

    long int    Id;
    Coord		entry;
	Coord		ptr;
    int         flags;

    // энергия задается при старте родителем и убывает по мере выполения операции
    // после ее исчерпания наступает смерть существа
    int64_t     energy;
    // энергия которую можно передать ребенку при рождении
    int64_t     child_energy;

    // стек и память виртуальной машины индивидуальны для каждого существа
	std::stack<Cell>	internal_stack;
	Coord				internal_memory[InternalMemoryCapacity];
    std::stack<Coord>	points;

    // отпечаток жизни существа - для статистики и подчистки территории после его смерти
    long int            lifetime;
    Fingerprint         fingerprint;

    // еще не рожденный ребенок, необходимо для очистки этих клеток
    // при смерти родителя до старта ребенка
    Fingerprint         embrion;

    //
    // поправка на мутации вставки/удаления
    // если вектор перехода операции указывает на точку "за" сохраненной точкой мутации
    // то в записываемый вектор вносится указанная поправка
    // при повторном проходе точки мутации поправка обнуляется
    //
    Coord       m_adjust;
    Coord       m_pos;
    bool        m_flag;

    Creature(Coord entryPoint, int64_t energy);
    ~Creature();

    void execInstruction();
    void applyPop(Coord &targetCoord, Cell &target, Instruction &value);
    void doAbort();

    void print();


    // debug & messaging
    int counter1;
    int counter2;
    char *msgbuf;

};


#endif

