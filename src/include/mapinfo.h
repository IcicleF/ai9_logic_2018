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
    MapInfo();
    ~MapInfo();
    std::vector<std::pair<int, Vec2> > calcCommands(GameLogic *caller);
    void placeWard(GameLogic *caller, int pid, Vec2 pos);
    void throwBomb(GameLogic *caller, int pid, Vec2 pos);
    void unitDied(GameLogic *caller, Vec2 pos);

    std::vector<PWardInfo> getWards(int pid);
    std::vector<Vec2> getCorpses();

private:
    struct WardInfo
    {
        int id;              //为了方便，给每个守卫也分配一个ID
        int owner;           //所有者ID
        Vec2 pos;            //位置
        int life;            //剩余时间（内部回合数）
    };
    std::list<WardInfo> wards;

    struct BombInfo
    {
        int owner;           //扔出该炸弹的玩家ID
        Vec2 pos;            //爆炸位置
        int life;            //剩余时间（内部回合数）
    };
    std::list<BombInfo> bombs;

    struct DeathInfo
    {
        int id;              //为了方便，给每个死亡痕迹也分配一个ID
        Vec2 pos;            //死亡痕迹的位置
        int life;            //剩余时间（内部回合数）
    };
    std::list<DeathInfo> deaths;
};

#endif  //MAPINFO_H