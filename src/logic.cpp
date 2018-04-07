#include <chrono>
#include <set>
#include <algorithm>

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
    for (int i = 0; i < InitialVillagers * playerCount; ++i)
    {
        int id = idManager.newID();
        unitInfo[id] = new Unit(this);
        unitInfo[id]->id = id;
        addCommand(UnitSpawn, id, unitInfo[id]->position);
    }

    //创建初始状态的json字符串
    translateCommands();
}

void GameLogic::reportActions(int pid, const Actions& _actions)
{
    for (const Action& act : _actions.actions)
		if (act.actionType != NoAction && act.actionType != ContinueMovement)
			actions.push_back(make_pair(pid, act));
}
vector<int> GameLogic::getIDs()
{
    return playerID;
}

#define DECLARE_MAPPING(parm, type) if ((parm) == (type)) return #type;
string GameLogic::getActionTypeName(ActionType t)
{
    DECLARE_MAPPING(t, SelectDestination);
    DECLARE_MAPPING(t, BuyItem);
    DECLARE_MAPPING(t, UseItem);
    DECLARE_MAPPING(t, SuckAttack);
    return "NoAction";
}
string GameLogic::getCommandTypeName(CommandType t)
{
    DECLARE_MAPPING(t, MoveTo)
    DECLARE_MAPPING(t, BombThrown)
    DECLARE_MAPPING(t, BombExplode)
    DECLARE_MAPPING(t, WardPlaced)
    DECLARE_MAPPING(t, WardExpired)
    DECLARE_MAPPING(t, CorpseExpired)
    DECLARE_MAPPING(t, BombNumberChange)
    DECLARE_MAPPING(t, WardNumberChange)
    DECLARE_MAPPING(t, GoldChange)
    DECLARE_MAPPING(t, HPChange)
    DECLARE_MAPPING(t, ScoreChange)
    DECLARE_MAPPING(t, PlayerAttack)
    DECLARE_MAPPING(t, UnitDie)
    DECLARE_MAPPING(t, UnitSpawn)
    DECLARE_MAPPING(t, DayNightSwitch)
	DECLARE_MAPPING(t, NightDaySwitch)
    DECLARE_MAPPING(t, GameSet)
    return "NoCommand";
}
#undef DECLARE_MAPPING

void GameLogic::translateCommands()
{
    Json::FastWriter writer;
    Json::Value root;

    root["time"] = getCurrentRound();

    Json::Value cmds;
    for (auto cmd : commands)
    {
        Json::Value jcmd;
        jcmd["typ"] = getCommandTypeName(cmd.commandType);
        jcmd["id"] = cmd.unit_id;
        jcmd["x"] = toJStr(cmd.pos.x);
        jcmd["y"] = toJStr(cmd.pos.y);
        jcmd["d"] = cmd.delta;
        if (cmd.commandType == BombThrown || cmd.commandType == WardPlaced || cmd.commandType == UnitDie)
            jcmd["tar"] = cmd.target_id;
        if (cmd.commandType == BombThrown)
        {
            jcmd["dx"] = toJStr(cmd.direction.x);
            jcmd["dy"] = toJStr(cmd.direction.y);
        }
        cmds.append(jcmd);
    }
    root["cmds"] = cmds;
    if (getCurrentRound() == 0)
    {
        jsoncmd = writer.write(root);
        return;
    }

#ifndef HIDE_ROUND_INFO
    Json::Value round;
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
    {
        Json::Value junit;
        Unit& unit = *(it->second);
        junit["typ"] = (unit.getUnitType() == PlayerType) ? "p" : "v";
        junit["id"] = it->first;
        junit["hp"] = unit.hp;
        junit["x"] = toJStr(unit.position.x);
        junit["y"] = toJStr(unit.position.y);
        junit["vx"] = toJStr(unit.velocity.x);
        junit["vy"] = toJStr(unit.velocity.y);
        if (unit.getUnitType() == PlayerType)
        {
            Player* pptr = dynamic_cast<Player*>(it->second);
            Player& pl = *pptr;
            junit["b"] = pl.bombCount;
            junit["bcd"] = pl.bombCD;
            junit["w"] = pl.wardCount;
            junit["wcd"] = pl.wardCD;
            junit["acd"] = pl.suckAttackCD;
            junit["s"] = pl.score;
        }
        round.append(junit);
    }
    auto info = mapInfo.getAllMapInfo();
    for (auto item : info)
    {
        Json::Value jinfo;
        jinfo["id"] = item.id;
        jinfo["x"] = toJStr(item.pos.x);
        jinfo["y"] = toJStr(item.pos.y);
        switch (item.life)
        {
            case 0:
                jinfo["typ"] = "b";
                jinfo["vx"] = toJStr(item.velocity.x);
                jinfo["vy"] = toJStr(item.velocity.y);
                jinfo["o"] = item.owner;
                break;
            case 1:
                jinfo["typ"] = "w";
                jinfo["o"] = item.owner;
                break;
            case 2:
                jinfo["typ"] = "c";
                break;
            default: break;
        }
        round.append(jinfo);
    }
    root["round"] = round;
#endif
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

    res.round = getCurrentRound();
    res.id = pid;
    res.hp = pl.hp;
    res.pos = pl.position;
    res.velocity = pl.velocity;

    res.gold = pl.gold;
    res.score = pl.score;
    res.continuousKill = pl.continuousKill;

    res.bombCount = pl.bombCount;
    res.wardCount = pl.wardCount;
    res.canUseBomb = pl.canUseBomb();
    res.canUseWard = pl.canUseWard();
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
    res.placedWard = wards;
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

    //炸弹
    vector<PBombInfo> bombs = mapInfo.getBombs();
    for (auto b : bombs)
    {
        bool flag = false;
        if ((b.pos - pl.position).length() < currentSight)
            flag = true;
        if (!flag)
            for (auto w : wards)
            {
                Vec2 wardPos = w.pos;
                if ((b.pos - wardPos).length() < WardRadius)
                {
                    flag = true;
                    break;
                }
            }
        if (flag)
            res.bombInSight.push_back(b);
    }
    res.bombInSightCount = res.bombInSight.size();

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
bool GameLogic::plcomp(const Player &x, const Player &y)
{
    if (x.score != y.score)
        return x.score < y.score;
    if (x.gold != y.gold)
        return x.gold < y.gold;
    if (x.killedVillager + x.killedEnemy != y.killedVillager + y.killedEnemy)
        return x.killedVillager + x.killedEnemy > y.killedVillager + y.killedEnemy;
    return x.killedVillager >= y.killedVillager;
}
int GameLogic::judgeWin(bool forced)
{
    Player* winner = nullptr;
    int res = -1;
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->getUnitType() == PlayerType)
        {
            Unit* uptr = it->second;
            Player* pptr = dynamic_cast<Player*>(uptr);

            if (winner == nullptr)
                winner = pptr, res = it->first;
            else if (plcomp(*winner, *pptr))
                winner = pptr, res = it->first;
        }
    if (forced || winner->score >= GameSetLimit)
    {
        addCommand(GameSet, -1);
        logicStat = GameEnded;
        return res;
    }
    return -1;
}
void GameLogic::getRank(int *rank)
{
    vector<Player*> pptrs;
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->getUnitType() == PlayerType)
            pptrs.push_back(dynamic_cast<Player *>(it->second));
    for (int i = 0; i < playerCount; ++i)
        for (int j = i + 1; j < playerCount; ++j)
            if (plcomp(*pptrs[i], *pptrs[j]))
            {
                Player* tmp = pptrs[i];
                pptrs[i] = pptrs[j];
                pptrs[j] = tmp;
            }
    for (int i = 0; i < playerCount; ++i)
    {
        cout << pptrs[i]->id << " " << pptrs[i]->score << " " << pptrs[i]->gold << " "
             << pptrs[i]->killedVillager << endl;
        rank[i] = pptrs[i]->id;
    }
    //cout << endl;
}

//新回合刷新
void GameLogic::refresh()
{
    ++round;
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        it->second->refresh();
}

/*
 *  回合的计算顺序：
 *  0. 向村民单位索取当前回合的行动；
 *  1. 发放工资；
 *  ***** 获取视野 *****
 *  2. 计算已经存在于地图上的炸弹的爆炸，以及守卫的消失；
 *  3. 如果是白天，则活着的玩家计算灼烧伤害；
 *  4. 计算普通攻击的伤害和吸血；（上面所有伤害是一起结算的）
 *  5. 结算玩家的死亡惩罚和击杀奖励（先惩罚再奖励）；
 *  6. 计算扔出炸弹和放置守卫，如果前面死亡则无效；
 *  7. 计算玩家移动，如果前面死亡则无效；
 *  8. 死亡惩罚结束的角色重生；
 *  9. 村民随机重生；
 *  10. 计算道具购买。
 */
void GameLogic::preCalc()
{
    this->refresh();

    //获取村民的移动
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        if (it->second->getUnitType() == VillagerType)
            it->second->getAction();

    //发放工资
    if (getCurrentRound() % SalaryPeriod == 0)
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
            if (it->second->getUnitType() == PlayerType)
            {
                Player *pptr = dynamic_cast<Player*>(it->second);
                pptr->gold += Salary;
                addCommand(GoldChange, pptr->id, Salary);
            }
}

void GameLogic::calcRound()
{
    logicStat = CalculatingRound;

    map<int, int> damages;
    map<int, set<int> > damageGiver;

    if (getCurrentRound() % (DayTime + NightTime) == 1 && getCurrentRound() != 1)
        addCommand(NightDaySwitch, -1);
	else if (getCurrentRound() % (DayTime + NightTime) == DayTime + 1)
		addCommand(DayNightSwitch, -1);

    //计算已经扔出的炸弹的爆炸
    vector<pair<int, Vec2> > explosions = mapInfo.calcCommands(this);
    for (auto e : explosions)
    {
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
            if (it->first != e.first && it->second->hp > 0)
                if ((it->second->position - e.second).length() < BombRadius)
                {
                    damages[it->first] += BombDamage;
                    damageGiver[it->first].insert(e.first);
                }
    }

    //计算灼烧伤害
    if (getCurrentDayPeriod() == Day && getCurrentRound() % BurnPeriod == 0)
        for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
        {
            Unit& unit = *(it->second);
            if (unit.getUnitType() != PlayerType || unit.hp == 0)
                continue;
            damages[it->first] += BurnDamage;
        }

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
            addCommand(PlayerAttack, id, target_id, SuckDamage, target.position, target.position - unit.position);
            damages[id] -= int(SuckDamage * SuckDrainCoeff);
            damages[target_id] += SuckDamage;
            damageGiver[target_id].insert(id);
        }

    //计算单位死亡和金钱奖励
    for (auto it = damages.begin(); it != damages.end(); ++it)
    {
        int id = it->first;
        int volume = it->second;
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
            int corpse_id = idManager.newID();
            addCommand(HPChange, id, -unit.hp);
			addCommand(UnitDie, id, corpse_id, 0, unit.position, Vec2());
            mapInfo.unitDied(this, corpse_id, unit.position);
            unit.hp = 0;
        }
    }
    for (auto it = damages.begin(); it != damages.end(); ++it)    //计算死亡惩罚
    {
        Unit& unit = *unitInfo[it->first];
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
            Vec2 pos = act.second.pos;
            if (!mapInfo.inRange(pos))
                continue;
            if (act.second.target_id == BombItem)    //炸弹
            {
                if (unit.hasBomb() && unit.canUseBomb())
                {
                    unit.useBomb();
                    int bomb_id = mapInfo.throwBomb(this, id, unit.position, pos);
                    addCommand(BombThrown, id, bomb_id, 0, unit.position, pos);
                }
            }
            else                                    //守卫
            {
                if (unit.hasWard() && unit.canUseWard() && (pos - unit.position).length() <= WardPlaceRadius)
                {
                    unit.useWard();
                    int ward_id = mapInfo.placeWard(this, id, pos);
                    addCommand(WardPlaced, id, ward_id, 0, unit.position, pos);
                }
            }
        }
    }

    //计算单位移动
    for (auto it = unitInfo.begin(); it != unitInfo.end(); ++it)
    {
        Unit& unit = *(it->second);
        unit.moved = false;
        if (unit.hp > 0 && unit.currentTarget != -1 && unit.currentTarget < unit.targets.size())
            actions.push_back(make_pair(it->first, Action(ContinueMovement)));
    }
    for (auto act : actions)
    {
        ActionType type = act.second.actionType;
        if (unitInfo.find(act.first) == unitInfo.end())
            continue;
        Unit &unit = *unitInfo[act.first];
        if (unit.hp == 0 || unit.moved)
            continue;
        if (type == SelectDestination || type == ContinueMovement)
        {
			if (type == ContinueMovement && unit.currentTarget == -1)
				continue;
            unit.moved = true;
            if (type == SelectDestination)
            {
                //cout << unit.id << " " << unit.position << " " << act.second.pos << endl;
                if (!Router::getInstance()->Reachable(act.second.pos))
                {
                    unit.targets.clear();
                    unit.currentTarget = -1;
                    continue;
                }
                unit.targets = Router::getInstance()->Route(unit.position, act.second.pos);
                unit.currentTarget = 0;
                //cout << unit.targets.size() << endl;
            }
            float sigma = PlayerVelocity;
            while (true)
            {
                if (unit.currentTarget == unit.targets.size())
                {
                    unit.velocity = Vec2();
                    unit.targets.clear();
                    unit.currentTarget = -1;
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
                    {
                        unit.velocity = Vec2();
                        unit.targets.clear();
                        unit.currentTarget = -1;
                        break;
                    }
                }
            }
            //cout << unit.id << " " << unit.position << " " << act.second.pos << endl << endl;
            addCommand(MoveTo, act.first, unit.position);
        }
    }

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

        addCommand(UnitSpawn, id, unitInfo[id]->position);
    }

    translateCommands();
    logicStat = DistributingCommands;
}