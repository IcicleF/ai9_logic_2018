//
// Created by IcyF on 2018/2/6.
//

#include "idmgr.h"
#include <chrono>

using namespace std;

//class: IDManager
IDManager::IDManager()
{
    current_id = (int)(chrono::system_clock::now().time_since_epoch().count() % 15);
}
int IDManager::newID()
{
    return current_id++;
}

//class: PlayerIDManager
PlayerIDManager::PlayerIDManager()
{
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    core = new mt19937(seed);
    idRnd = new uniform_int_distribution<int>(0, 8192);

    relation.clear();
    backtrace.clear();
    updated.clear();
    used.clear();
}
int PlayerIDManager::newID()
{
    int rnd = (*idRnd)(*core);
    while (used.find(rnd) != used.end())
        rnd = (*idRnd)(*core);
    used.insert(rnd);
    return rnd;
}
void PlayerIDManager::declareVisible(int id)
{
    if (relation.find(id) != relation.end())
        updated[id] = relation[id];
    else
        updated[id] = this->newID();
}
void PlayerIDManager::replace(int src, int dst)
{
    if (relation.find(src) == relation.end())
        return;
    int pid = relation[src];

    relation.erase(src);
    relation[dst] = pid;
    backtrace[pid] = dst;
}
void PlayerIDManager::update()
{
    relation = updated;
    updated.clear();
    backtrace.clear();
    for (auto it = relation.begin(); it != relation.end(); ++it)
        backtrace[it->second] = it->first;
}