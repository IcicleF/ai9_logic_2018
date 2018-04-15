//
// Created by IcyF on 2018/2/6.
//

#include "idmgr.h"

//class: IDManager
IDManager::IDManager()
{
    current_id = 10;
}
int IDManager::newID()
{
    return current_id++;
}

//class: PlayerIDManager
PlayerIDManager::PlayerIDManager()
{
    current_id = 0;
    relation.clear();
    backtrace.clear();
    updated.clear();
}
void PlayerIDManager::declareVisible(int id)
{
    if (relation.find(id) != relation.end())
        updated[id] = relation[id];
    else
        updated[id] = ++current_id;
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