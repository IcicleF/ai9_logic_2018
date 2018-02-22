#ifndef MAPINFO_H
#define MAPINFO_H

#include <list>
#include <vector>
#include <utility>

#include "idmgr.h"
#include "sdk/defs.h"

class GameLogic;
class MapInfo
{
public:
    MapInfo() = default;
    ~MapInfo() = default;

    bool inRange(Vec2);

    std::vector<std::pair<int, Vec2> > calcCommands(GameLogic *caller);
    void placeWard(GameLogic *caller, int pid, Vec2 pos);
    void throwBomb(GameLogic *caller, int pid, Vec2 start, Vec2 end);
    void unitDied(GameLogic *caller, Vec2 pos);

    std::vector<PWardInfo> getWards(int pid);
    std::vector<PBombInfo> getBombs();
    std::vector<Vec2> getCorpses();

private:
    struct ItemInfo
    {
        int id;              //为了方便，给每个道具也分配一个ID
        int owner;           //所有者ID
        Vec2 pos;            //位置
        Vec2 velocity;       //炸弹的速度
        int life;            //剩余时间（内部回合数）
    };
    std::list<ItemInfo> wards, bombs;

    struct DeathInfo
    {
        int id;              //为了方便，给每个死亡痕迹也分配一个ID
        Vec2 pos;            //死亡痕迹的位置
        int life;            //剩余时间（内部回合数）
    };
    std::list<DeathInfo> deaths;
};

#endif  //MAPINFO_H