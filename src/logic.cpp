#include <chrono>
#include <set>
#include <algorithm>
#include <cstring>

#include "logic.h"
#include "json/json.h"

using namespace std;

string toJStr(float val)
{
    string res = to_string(val);
    return res.substr(0, res.find('.') + 3);
}

void GameLogic::startGame(int _playerCount)         //初始化游戏，设置玩家人数
{
    //初始化回合记录
    logicStat = DistributingCommands;
    round = 0;
    clearRoundRecord();

    //为每个玩家预分配ID，初始化玩家信息
    playerCount = _playerCount;
    playerID.resize(playerCount);
    for (int i = 0; i < playerCount; ++i)
    {
        playerID[i] = idManager.newID();
        unitInfo[playerID[i]] = new Player(this);
        unitInfo[playerID[i]]->id = playerID[i];
        addCommand(UnitSpawn, playerID[i], unitInfo[playerID[i]]->position);
    }

    //创建初始村民
    for (int i = 0; i < InitialVillagers; ++i)
    {
        int id = idManager.newID();
        unitInfo[id] = new Unit(this);
        unitInfo[id]->id = id;
        addCommand(UnitSpawn, id, unitInfo[id]->position);

        //初始状态的随机等待时间
        int hajime = Randomizer::getInstance()->randWaitTime() - (WanderIntervalLB + WanderIntervalUB) / 2;
        if (hajime > 0)
        {
            unitInfo[id]->currentTarget = -1;
            unitInfo[id]->waitUntil = hajime + 1;
        }
    }

    //创建初始状态的json字符串
    translateCommands();
}

void GameLogic::reportActions(int pid, const Actions& _actions)
{
    if (actionReceived.find(pid) != actionReceived.end())
        return;
    actionReceived.insert(pid);
    for (const Action& act : _actions.actions)
        this->actions.push_back(make_pair(pid, act));
}
vector<int> GameLogic::getIDs()
{
    return playerID;
}

string GameLogic::getCommandTypeName(CommandType t)
{
#define DECLARE_MAPPING(parm, type) if ((parm) == (type)) return #type;
    DECLARE_MAPPING(t, NoCommand)
    DECLARE_MAPPING(t, MoveTo)
    DECLARE_MAPPING(t, BombThrown)
    DECLARE_MAPPING(t, BombExplode)
    DECLARE_MAPPING(t, WardPlaced)
    DECLARE_MAPPING(t, WardExpired)
    DECLARE_MAPPING(t, CorpseAppear)
    DECLARE_MAPPING(t, CorpseExpired)
    DECLARE_MAPPING(t, BombNumberChange)
    DECLARE_MAPPING(t, WardNumberChange)
    DECLARE_MAPPING(t, GoldChange)
    DECLARE_MAPPING(t, HPChange)
    DECLARE_MAPPING(t, ScoreChange)
    DECLARE_MAPPING(t, UnitDie)
    DECLARE_MAPPING(t, UnitSpawn)
    DECLARE_MAPPING(t, GameSet)
#undef DECLARE_MAPPING
    return "NoCommand";
}
void GameLogic::translateCommands()
{
    Json::Value root;
    for (auto cmd : commands)
    {
        Json::Value jcmd;
        jcmd["s"] = getCurrentRound();
        jcmd["t"] = getCommandTypeName(cmd.commandType);
        jcmd["id"] = cmd.unit_id;
        jcmd["x"] = toJStr(cmd.pos.x);
        jcmd["y"] = toJStr(cmd.pos.y);
        jcmd["d"] = cmd.delta;
        root.append(jcmd);
    }
    Json::FastWriter writer;
    jsoncmd = writer.write(root);
}
string GameLogic::getCommands()
{
    return jsoncmd;
}

/*
 *  视野的计算：
 *  O(N)枚举所有单位，检测是否在侦查守卫或者本人的视野范围内。
 */
PlayerSight GameLogic::getSight(int pid)
{
    PlayerSight res;
    if (unitInfo.find(pid) == unitInfo.end())
        return res;
    Unit *uptr = unitInfo[pid];
    if (uptr->getUnitType() != PlayerType)
        return res;

    Player *pptr = dynamic_cast<Player*>(uptr);
    Player &pl = *pptr;

    PlayerIDManager& mgr = playerIDMgr[pid];
    float currentSight = getCurrentDayPeriod() == Day ? DaySight : NightSight;

    res.id = pid;
    res.hp = pl.hp;
    res.pos = pl.position;
    res.velocity = pl.velocity;

    res.gold = pl.gold;
    res.score = pl.score;
    res.continuousKill = pl.continuousKill;

    res.bombCount = pl.bombCount;
    res.wardCount = pl.wardCount;
    res.canBuyBomb = pl.canBuyBomb();
    res.canBuyWard = pl.canBuyWard();
    res.canSuckAttack = pl.canSuckAttack();

    //更新PlayerIDManager
    if (pl.hp > 0)    //如果已经死亡那么丧失自身的视野（守卫不受影响）
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        {
            Unit &other = *(it->second);
            if (it->first == pid || other.hp == 0)
                continue;
            if ((other.position - pl.position).length() < currentSight)
                mgr.declareVisible(it->first);
        }
    vector<PWardInfo> wards = mapInfo.getWards(pid);
    for (auto w : wards)
    {
        Vec2 wardPos = w.pos;
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        {
            Unit &other = *(it->second);
            if (it->first == pid || other.hp == 0)
                continue;
            if ((other.position - wardPos).length() < WardRadius)
                mgr.declareVisible(it->first);
        }
    }
    mgr.update();

    //守卫
    res.ward = wards;
    res.placedWardCount = wards.size();

    //活人
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
    {
        Unit& other = *(it->second);
        if (mgr.hasOriginID(it->first))
        {
            PUnitInfo p;
            p.id = mgr.getPlayerID(it->first);
            p.pos = other.position;
            p.velocity = other.velocity;
            res.unitInSight.push_back(p);
        }
    }
    res.unitInSightCount = res.unitInSight.size();

    //尸体
    vector<Vec2> corpses = mapInfo.getCorpses();
    for (auto c : corpses)
    {
        bool flag = false;
        if ((c - pl.position).length() < currentSight)
            flag = true;
        if (!flag)
            for (auto w : wards)
            {
                Vec2 wardPos = w.pos;
                if ((c - wardPos).length() < WardRadius)
                {
                    flag = true;
                    break;
                }
            }
        if (flag)
            res.corpseInSight.push_back(c);
    }
    res.corpseInSightCount = res.corpseInSight.size();

    //计分板
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
    {
        Unit* o_uptr = it->second;
        if (o_uptr->getUnitType() != PlayerType)
            continue;
        Player* o_pptr = dynamic_cast<Player*>(uptr);
        Player& o_pl = *o_pptr;
        res.scoreBoard.push_back(o_pl.score);
    }
    sort(res.scoreBoard.begin(), res.scoreBoard.end());
    reverse(res.scoreBoard.begin(), res.scoreBoard.end());

    return res;
}

/*
 *  计算胜利者，返回其ID；如果尚未产生胜利者那么返回-1。
 *  如果forced = true那么强制产生一位胜利者。
 *
 *  比较条件：
 *  1. 先比较分数，分数大者胜出；
 *  2. 分数一样，比较金钱，金钱多者胜出；
 *  3. 金钱一样，比较总击杀数，总击杀数少者胜出；
 *  4. 总击杀数一样，比较击杀村民数，击杀村民数少者胜出；
 *  5. 如果还是一样，那么就随机= =。
 */
int GameLogic::judgeWin(bool forced)
{
    int maxScore = 0, maxGold = 0, minKills = 0, minVillagerKill = 0;
    int winner = -1;        //ID
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->getUnitType() == PlayerType)
        {
            Unit* uptr = it->second;
            Player* pptr = dynamic_cast<Player*>(uptr);
            Player& pl = *pptr;

            bool flag = false;
            if (winner == -1)
                flag = true;
            else if (maxScore < pl.score)
                flag = true;
            else if (maxScore == pl.score)
            {
                if (maxGold < pl.gold)
                    flag = true;
                else if (maxGold == pl.gold)
                {
                    if (minKills > pl.killedVillager + pl.killedEnemy)
                        flag = true;
                    else if (minKills == pl.killedVillager + pl.killedEnemy)
                    {
                        if (minVillagerKill > pl.killedVillager)
                            flag = true;
                    }
                }
            }
            if (flag)
            {
                winner = it->first;
                maxScore = pl.score;
                maxGold = pl.gold;
                minKills = pl.killedEnemy + pl.killedVillager;
                minVillagerKill = pl.killedVillager;
            }
        }
    if (forced || maxScore >= GameSetLimit)
    {
        addCommand(GameSet, -1);
        logicStat = GameEnded;
        return winner;
    }
    return -1;
}

//新回合刷新
void GameLogic::refresh()
{
    clearRoundRecord();
    ++round;
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        it->second->refresh();
}

/*
 *  回合的计算顺序：
 *  0. 向村民单位索取当前回合的行动；
 *  1. 计算已经存在于地图上的炸弹的爆炸，以及守卫的消失；
 *  2. 如果是白天，则活着的玩家计算灼烧伤害；
 *  3. 计算普通攻击的伤害和吸血；（上面所有伤害是一起结算的）
 *  4. 结算玩家的死亡惩罚和击杀奖励（先惩罚再奖励）；
 *  5. 计算扔出炸弹和放置守卫，如果前面死亡则无效；
 *  6. 计算玩家移动，如果前面死亡则无效；
 *  7. 死亡惩罚结束的角色重生；
 *  8. 村民随机重生；
 *  9. 计算道具购买。
 */
void GameLogic::calcRound()
{
    logicStat = CalculatingRound;
    this->refresh();

    map<int, int> damages;
    map<int, set<int> > damageGiver;

    //获取村民的移动
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->getUnitType() == VillagerType)
            it->second->getAction();

    //cout << " - - calcRound: villager movement fetched." << endl;

    //计算已经扔出的炸弹的爆炸
    vector<pair<int, Vec2> > explosions = mapInfo.calcCommands(this);
    for (auto e : explosions)
    {
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
            if (it->second->id != e.first && it->second->hp > 0)
                if ((it->second->position - e.second).length() < BombRadius)
                {
                    damages[it->second->id] += BombDamage;
                    damageGiver[it->second->id].insert(e.first);
                }
        addCommand(BombExplode, 0, e.second);
    }

    //cout << " - - calcRound: bomb explosion calculated." << endl;

    //计算灼烧伤害
    if (getCurrentDayPeriod() == Day && getCurrentRound() % BurnPeriod == 0)
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        {
            Unit& unit = *(it->second);
            if (unit.getUnitType() != PlayerType || unit.hp == 0)
                continue;

            damages[it->first] += BurnDamage;
        }

    //cout << " - - calcRound: burn damage calculated." << endl;

    //处理普通攻击
    for (auto act : actions)
        if (act.second.actionType == SuckAttack)
        {
            int id = act.first;
            if (unitInfo.find(id) == unitInfo.end())
                continue;
            Unit& unit = *unitInfo[id];
            if (unit.getUnitType() != PlayerType || unit.hp == 0)
                continue;
            if (!unit.canSuckAttack())
                continue;

            int target_masked_id = act.second.target_id;
            PlayerIDManager& mgr = playerIDMgr[id];
            if (!mgr.hasPlayerID(target_masked_id))
                continue;
            int target_id = mgr.getOriginID(target_masked_id);
            if (unitInfo.find(target_id) == unitInfo.end())
                continue;
            Unit& target = *unitInfo[target_id];
            if (target.hp == 0 || target.hp <= damages[target.id])
                continue;
            if ((target.position - unit.position).length() > SuckRange)
                continue;

            unit.suckAttack();
            damages[id] -= int(SuckDamage * SuckDrainCoeff);
            damages[target_id] += SuckDamage;
            damageGiver[target_id].insert(id);
        }

    //cout << " - - calcRound: suck damage calculated." << endl;

    //计算单位死亡和金钱奖励
    for (auto it = damages.begin(); it != damages.end(); ++it)
    {
        int id = it->first;
        int& volume = it->second;
        Unit& unit = *unitInfo[id];
        if (unit.hp > volume)       //回复效果，或者伤害尚未致死
        {
            if (unit.hp - volume > PlayerHP)    //回复效果超出上限
                volume = unit.hp - PlayerHP;
            addCommand(HPChange, id, -volume);
            unit.hp -= volume;
        }
        else                            //伤害致死；死亡角色的后处理随后进行
        {
            addCommand(HPChange, id, -unit.hp);
            mapInfo.unitDied(this, unit.position);
            unit.hp = 0;
        }
    }
    for (auto it = damages.begin(); it != damages.end(); ++it)    //计算死亡惩罚
    {
        Unit& unit = *unitInfo[it->first];
        addCommand(UnitDie, it->first, unit.position);
        if (unit.getUnitType() == PlayerType && unit.hp == 0)
        {
            unit.hp = 0;
            unit.respawnWhen = getCurrentRound() + RespawnTime;
            unit.velocity = Vec2();
            unit.currentTarget = -1;
            unit.targets.clear();
            unit.receiveDeathPunishment();
        }
    }
    for (auto it = damages.begin(); it != damages.end(); ++it)      //计算击杀奖励
    {
        Unit& unit = *unitInfo[it->first];
        if (unit.hp == 0)
            for (auto id : damageGiver[it->first])
            {
                Unit* srcptr = unitInfo[id];
                if (srcptr->getUnitType() != PlayerType)
                {
                    cout << "Damage Calculation Error!" << endl;
                    exit(0);
                }
                Player* srcplptr = dynamic_cast<Player*>(srcptr);
                Player& src = *srcplptr;

                if (unit.getUnitType() == VillagerType)
                {
                    src.gold += GoldPerVillager;
                    ++src.killedVillager;
                    addCommand(GoldChange, id, GoldPerVillager);
                }
                else
                {
                    src.gold += GoldPerEnemy;
                    src.score += (++src.continuousKill) * EnemyKillBonus;
                    ++src.killedEnemy;
                    addCommand(GoldChange, id, GoldPerEnemy);
                    addCommand(ScoreChange, id, src.continuousKill * EnemyKillBonus);
                }
            }
    }
    for (auto it = damages.begin(); it != damages.end(); ++it)      //清除死亡角色的连杀记录
    {
        Unit& unit = *unitInfo[it->first];
        if (unit.getUnitType() == PlayerType && unit.hp == 0)
            unit.clearContinuousKill();
    }
    for (auto it = unitInfo.begin(); it != unitInfo.end(); )        //清除死亡的村民
    {
        Unit& unit = *(it->second);
        if (unit.hp == 0 && unit.getUnitType() == VillagerType)
        {
            delete (it->second);
            it = unitInfo.erase(it);
        }
        else
            ++it;
    }

    //cout << " - - calcRound: deaths calculated." << endl;

    //计算单位移动
    for (auto act : actions)
    {
        ActionType type = act.second.actionType;
        if (unitInfo.find(act.first) == unitInfo.end())
            continue;
        Unit &unit = *unitInfo[act.first];
        if (unit.hp == 0)
            continue;
        if (type == SelectDestination || type == ContinueMovement)
        {
            if (type == SelectDestination)
            {
                if (!Router::getInstance()->Reachable(act.second.pos))
                {
                    unit.targets.clear();
                    unit.currentTarget = -1;
                    continue;
                }
                unit.targets = Router::getInstance()->Route(unit.position, act.second.pos);
                unit.currentTarget = 0;
            }
            float sigma = PlayerVelocity;
            while (true)
            {
                if (unit.currentTarget == unit.targets.size())
                {
                    unit.velocity = Vec2();
                    break;
                }
                Vec2 direction = unit.targets[unit.currentTarget] - unit.position;
                Vec2 e0 = direction / direction.length();
                if (direction.length() - sigma < -EPS)      //走到目标地点还有剩余
                    unit.position = unit.targets[unit.currentTarget++];
                else if (direction.length() - sigma > EPS)  //走不到目标地点
                {
                    unit.velocity = e0 * PlayerVelocity;
                    unit.position = unit.position + (e0 * sigma);
                    break;
                }
                else                                        //恰好走到目标地点
                {
                    unit.position = unit.targets[unit.currentTarget++];
                    if (unit.currentTarget < unit.targets.size())
                    {
                        direction = unit.targets[unit.currentTarget] - unit.position;
                        e0 = direction / direction.length();
                        unit.velocity = e0 * PlayerVelocity;
                    }
                    else
                        unit.velocity = Vec2();
                    break;
                }
            }
            addCommand(MoveTo, act.first, unit.position);
        }
    }

    //cout << " - - calcRound: all movement calculated." << endl;

    //处理扔出炸弹和放置守卫
    for (auto act : actions)
    {
        int id = act.first;
        if (unitInfo.find(id) == unitInfo.end())
            continue;
        if (act.second.actionType == UseItem)
        {
            Unit &unit = *unitInfo[id];
            if (unit.getUnitType() != PlayerType || unit.hp == 0 || unit.hp <= damages[unit.id])
                continue;
            if (act.second.target_id == 0)          //炸弹
            {
                if (unit.canUseBomb())
                {
                    unit.useBomb();
                    addCommand(BombThrown, id, act.second.pos);
                    mapInfo.throwBomb(this, id, act.second.pos);
                }
            }
            else                                    //守卫
            {
                if (unit.canUseWard())
                {
                    unit.useWard();
                    addCommand(WardPlaced, id, act.second.pos);
                    mapInfo.placeWard(this, id, act.second.pos);
                }
            }
        }
    }

    //cout << " - - calcRound: item use calculated." << endl;

    //死亡惩罚结束的玩家重生
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->hp == 0 && it->second->respawnWhen == getCurrentRound())
        {
            Unit& respawner = *(it->second);
            respawner.respawnWhen = -1;
            respawner.hp = PlayerHP;
            respawner.velocity = Vec2();
            respawner.position = Router::getInstance()->availablePosition();

            addCommand(UnitSpawn, respawner.id, respawner.position);
        }

    //村民随机重生
    while (Randomizer::getInstance()->randSpawnJudger())
    {
        int id = idManager.newID();
        unitInfo[id] = new Unit(this);
        unitInfo[id]->id = id;
    }

    //cout << " - - calcRound: respawn calculated." << endl;

    //计算道具的购买
    for (auto act : actions)
        if (act.second.actionType == BuyItem)
        {
            int id = act.first;
            if (unitInfo.find(id) == unitInfo.end())
                continue;
            Unit& unit = *unitInfo[act.first];
            if (unit.getUnitType() != PlayerType || unit.hp == 0)
                continue;
            if (act.second.target_id == 0)          //炸弹
            {
                if (unit.canBuyBomb())
                {
                    unit.buyBomb();
                    addCommand(BombNumberChange, id, 1);
                    addCommand(GoldChange, id, -BombPrice);
                }
            }
            else                                    //守卫
            {
                if (unit.canBuyWard())
                {
                    unit.buyWard();
                    addCommand(WardNumberChange, id, 1);
                    addCommand(GoldChange, id, -WardPrice);
                }
            }
        }

    //cout << " - - calcRound: item buy calculated. [ALL FINISHED]" << endl;
    translateCommands();
    logicStat = DistributingCommands;
}