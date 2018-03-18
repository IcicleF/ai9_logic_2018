#include "mapinfo.h"
#include "logic.h"

using namespace std;

bool MapInfo::inRange(Vec2 p)
{
    return p.x >= 0 && p.x <= MapWidth && p.y >= 0 && p.y <= MapHeight;
}
vector<pair<int, Vec2> > MapInfo::calcCommands(GameLogic *caller)
{
    //清除所有到期的守卫
    for (auto it = wards.begin(); it != wards.end(); )
    {
        --(it->life);
        if (it->life == 0)
        {
            caller->addCommand(WardExpired, it->id, it->pos);
            it = wards.erase(it);
        }
        else
            ++it;
    }

    //清除所有到期的尸体
    for (auto it = deaths.begin(); it != deaths.end(); )
    {
        --(it->life);
        if (it->life == 0)
        {
            caller->addCommand(CorpseExpired, it->id, it->pos);
            it = deaths.erase(it);
        }
        else
            ++it;
    }

    //炸弹向前飞行
    for (auto it = bombs.begin(); it != bombs.end(); ++it)
    {
        it->pos = it->pos + it->velocity;
        caller->addCommand(MoveTo, it->id, it->pos);
    }

    //记录所有到期的炸弹，引爆工作交由主逻辑进行
    vector<pair<int, Vec2> > explosions;
    for (auto it = bombs.begin(); it != bombs.end(); )
    {
        --(it->life);
        if (it->life == 0)
        {
            explosions.emplace_back(it->owner, it->pos);
            caller->addCommand(BombExplode, it->id, it->pos);
            it = bombs.erase(it);
        }
        else
            ++it;
    }

    return explosions;
}
int MapInfo::placeWard(GameLogic *caller, int pid, Vec2 pos)
{
    ItemInfo w;
    w.id = caller->idManager.newID();
    w.owner = pid;
    w.pos = pos;
    w.velocity = Vec2();
    w.life = WardDuration;
    wards.push_back(w);
    return w.id;
}
int MapInfo::throwBomb(GameLogic *caller, int pid, Vec2 start, Vec2 end)
{
    ItemInfo b;
    b.id = caller->idManager.newID();
    b.owner = pid;
    b.pos = start;
    b.life = BombTrajectoryTime;
    b.velocity = (end - start) / BombTrajectoryTime;
    bombs.push_back(b);
    return b.id;
}
void MapInfo::unitDied(GameLogic *caller, int id, Vec2 pos)
{
    DeathInfo d;
    d.id = id;
    d.pos = pos;
    d.life = CorpseLifeTime;
    deaths.push_back(d);
}

vector<PWardInfo> MapInfo::getWards(int pid)
{
    vector<PWardInfo> res;
    for (auto it = wards.begin(); it != wards.end(); ++it)
        if (it->owner == pid)
        {
            PWardInfo p;
            p.life = it->life;
            p.pos = it->pos;
            res.push_back(p);
        }
    return res;
}
vector<PBombInfo> MapInfo::getBombs()
{
    vector<PBombInfo> res;
    for (auto it = bombs.begin(); it != bombs.end(); ++it)
    {
        PBombInfo p;
        p.pos = it->pos;
        p.velocity = it->velocity;
        res.push_back(p);
    }
    return res;
}
vector<Vec2> MapInfo::getCorpses()
{
    vector<Vec2> res;
    for (auto it = deaths.begin(); it != deaths.end(); ++it)
        res.push_back(it->pos);
    return res;
}

vector<ItemInfo> MapInfo::getAllMapInfo()
{
    vector<ItemInfo> res;
    //Use life as the type
    for (auto w : wards)
    {
        ItemInfo i = w;
        i.life = 0;
        res.push_back(i);
    }
    for (auto b : bombs)
    {
        ItemInfo i = b;
        b.life = 1;
        res.push_back(i);
    }
    for (auto c : deaths)
    {
        ItemInfo i;
        i.id = c.id;
        i.pos = c.pos;
        i.life = 2;
        res.push_back(i);
    }
    return res;
}