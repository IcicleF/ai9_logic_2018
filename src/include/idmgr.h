#ifndef IDMGR_H
#define IDMGR_H

#include <map>
#include "sdk/defs.h"

//为每个单位分配UID
class IDManager
{
public:
    IDManager();
    int newID();

private:
    int current_id;
};

//管理每个玩家能“看见”的单位的“主观ID”
class PlayerIDManager
{
public:
    PlayerIDManager();
    void declareVisible(int id);
    void update();

    void replace(int src, int dst);

    bool hasOriginID(int oid) { return relation.find(oid) != relation.end(); }
    bool hasPlayerID(int pid) { return backtrace.find(pid) != backtrace.end(); }
    int getPlayerID(int oid) { return relation[oid]; }
    int getOriginID(int pid) { return backtrace[pid]; }

private:
    std::map<int, int> relation, backtrace;   //维护实际编号和玩家看到编号的对应关系
    std::map<int, int> updated;               //用于内部回合刷新时更新编号
    int current_id;                           //下次将要指派的新编号，每次自增1
};

#endif //IDMGR_H
