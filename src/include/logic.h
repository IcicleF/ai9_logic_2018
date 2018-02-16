#ifndef LOGIC_H
#define LOGIC_H

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <random>
#include <iostream>
#include <string>

#include "sdk/vec2.h"
#include "sdk/defs.h"
#include "sdk/actions.h"
#include "sdk/randomizer.h"
#include "sdk/router.h"
#include "idmgr.h"
#include "unit.h"
#include "mapinfo.h"

class GameLogic
{
    friend class MapInfo;
public:
    GameLogic() { logicStat = NotInitialized; }
    ~GameLogic() = default;
    LogicStatus getLogicStatus() { return logicStat; }

    void startGame(int _playerCount);        //初始化游戏，设置玩家人数
    std::vector<int> getIDs();
    PlayerSight getSight(int pid);

    int getCurrentRound() { return round; }
    DayPeriod getCurrentDayPeriod()
    {
        if (getCurrentRound() % (DayTime + NightTime) < DayTime)
            return Day;
        else
            return Night;
    }

    void startReporting()
    {
        if (logicStat != DistributingCommands)
            return;
        logicStat = WaitingPlayerActions;
    }
    void reportActions(int pid, const Actions& _actions);
    void calcRound();
    int judgeWin(bool forced = false);
    std::string getCommands();

    IDManager idManager;
    MapInfo mapInfo;

private:
    LogicStatus logicStat;
    int round;

    int playerCount;
    std::vector<int> playerID;
    std::map<int, Unit*> unitInfo;
    std::map<int, PlayerIDManager> playerIDMgr;

    std::set<int> actionReceived;

    std::vector<std::pair<int, Action> > actions;
    std::vector<Command> commands;
    std::string jsoncmd;

    void addCommand(CommandType type, int id, Vec2 pos = Vec2())
    {
        Command cmd(type);
        cmd.unit_id = id;
        cmd.delta = 0;
        cmd.pos = pos;
        commands.push_back(cmd);
    }
    void addCommand(CommandType type, int id, int delta, Vec2 pos = Vec2())
    {
        Command cmd(type);
        cmd.unit_id = id;
        cmd.delta = delta;
        cmd.pos = pos;
        commands.push_back(cmd);
    }
    void translateCommands();
    void refresh();
    void clearRoundRecord()
    {
        actionReceived.clear();
        actions.clear();
        commands.clear();
    }
    std::string getCommandTypeName(CommandType);
};

#endif  //LOGIC_H