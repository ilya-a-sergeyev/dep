#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <stack>
#include <algorithm>
#include <random>

#include <sys/time.h>

#include "World.h"

static unsigned long time_msec();

int32_t World::worldSize = 0;
World   *World::instance = 0;

World *World::getInstance()
{
    if (!instance)
        instance = new World(WORLD_SIZE);

    return instance;
}

World::~World()
{
    for(auto t:creatures) {
		Creature *tmp = t;
		if (tmp) {
			delete tmp;
		}
	}
    creatures.clear();
}

World::World(int32_t size)
{
    Log::Not << "Hello!" << log4cpp::eol;

    std::random_device _rd;
    std::mt19937 _gen(_rd());
    std::uniform_int_distribution<> _dis(0, 3);

    // кое-какие инициализации
    Instruction::prepare_instructions();

    worldSize = size;
    soup.reserve(worldSize*worldSize);
    for (int i=0; i<worldSize*worldSize; i++) {
        soup[i].clear();
        soup[i].dir = static_cast<Direction>(_dis(_gen));
    }

    Log::Not << worldSize << "x" << worldSize << " toroidal world allocated" << log4cpp::eol;
}

int World::LoadCreature(Coord point, Model &model, int32_t init_energy)
{
    Coord ptr = point;

    Creature *newcomer = new Creature(point, init_energy);

    for (unsigned int i=0;i<model.size();i++) {
        Cell &tgt = getCell(ptr.x, ptr.y);
        tgt.instruction = model[i].ins;
        tgt.dir = model[i].dir;
        newcomer->fingerprint.push_back(cellIdx(ptr.x, ptr.y));
        ptr = ptr.next(model[i].dir);
    }

    Log::Not << "Uploaded chain of " << model.size() << " instructions." << log4cpp::eol;

    creatures.push_back(newcomer);

    return 0;
}

unsigned long World::cellIdx(uint32_t x, uint32_t y)
{
    return x%worldSize+(y%worldSize)*worldSize;
}

Cell &World::getCell(uint32_t x, uint32_t y)
{
    return soup[cellIdx(x, y)];
}

Cell &World::getCell(unsigned long idx)
{
    return soup[idx];
}

Cell &World::getCell(Coord &crd)
{
    return soup[cellIdx(crd.x, crd.y)];
}

void World::run(int counter)
{
    unsigned long start_time = time_msec();
    unsigned long instructions_cnt = 0;
    long i;
    unsigned int  cr_count_peak = 0;

    std::list<Creature *> to_die_list;

    std::cout << "Starting ..." << creatures.size() << "\n";
    Log::Not << " " << log4cpp::eol;

    for (i=0;i<counter && creatures.size()>0; ++i) {

        to_die_list.clear();

        for (Creature *crtc: creatures) {
            crtc->execInstruction();
            instructions_cnt++;
            if (crtc->energy <0) {
                to_die_list.push_back(crtc);
            }
        }

        for (Creature *crtd: to_die_list) {
            long int Id = crtd->Id;
            auto crd = std::find_if(creatures.begin(), creatures.end(), [&Id](const Creature *crt1) { return crt1->Id == Id; });
            creatures.erase(crd);
            delete crtd;
        }

        if (i%1000 == 0) {
            std::cout << "After loop " << i << " we have " << creatures.size() << " creatures\n";
        }

        cr_count_peak = cr_count_peak>creatures.size()?cr_count_peak:creatures.size();
    }

    unsigned long total_time = time_msec() - start_time;

    std::cout << "\n";
    std::cout << "Total.................." << instructions_cnt << " instructions (" << i <<" loops).\n";
    std::cout << "Rate is about.........." << (total_time?(instructions_cnt/1000)/total_time:0L) << " instr/msec\n";
    std::cout << "Last creature ID......." << Creature::CrIdCnt << "\n";

    unsigned long empty_cells = 0;
    unsigned long embrional_cells = 0;
    unsigned long corps_cells = 0;

    for (i=0; i<worldSize*worldSize; i++) {
        Cell &cell = soup[i];
        if (cell.instruction.code == Op_None) {
            empty_cells++;
        }
        else {
            if (cell.executorId == 0) {
                embrional_cells++;
            }
            else {
                corps_cells++;
            }
        }
    }

    std::cout << "Total creatures........" << creatures.size() << "\n";
    std::cout << "Peak creatures was....." << cr_count_peak << "\n";
    std::cout << ".......................\n";
    std::cout << "Total cells ..........." << World::worldSize*World::worldSize << "\n";
    std::cout << ".......................\n";
    std::cout << "Empty cells count......" << empty_cells << "\n";
    std::cout << "Embrion cells count...." << embrional_cells << "\n";
    std::cout << "Creatures cells count.." << corps_cells << "\n";

}

void World::startCreature(Creature* newcomer)
{
    creatures.push_back(newcomer);
}

static unsigned long time_msec()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    unsigned long usec = (unsigned long)tp.tv_sec*1000000UL + (unsigned long)tp.tv_usec;
    return (unsigned long)usec/1000UL;
}
