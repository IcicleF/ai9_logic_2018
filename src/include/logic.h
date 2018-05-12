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
public:
    GameLogic() { logicStat = NotInitialized; }
    ~GameLogic() = default;
    LogicStatus getLogicStatus() { return logicStat; }

    void startGame(int _playerCount);        //初始化游戏，设置玩家人数
	int getPlayerCount() { return playerCount; }
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
        clearRoundRecord();
        logicStat = WaitingPlayerActions;
    }
    void reportActions(int pid, const Actions& _actions);
    void preCalc();
    void calcRound();
    int judgeWin(bool forced = false);
    void getRank(int* rank, int* sc);

    void addCommand(CommandType type, int id, Vec2 pos = Vec2())
    {
        addCommand(type, id, -1, 0, pos, Vec2());
    }
    void addCommand(CommandType type, int id, int delta, Vec2 pos = Vec2())
    {
        addCommand(type, id, -1, delta, pos, Vec2());
    }
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

    std::vector<std::pair<int, Action> > actions;
    std::vector<Command> commands;
    std::string jsoncmd;

    void translateCommands();
    void refresh();
    void clearRoundRecord()
    {
        //std::cout << this->round << std::endl;
        actions.clear();
        commands.clear();
    }

    void addCommand(CommandType type, int unit_id, int target_id, int delta, Vec2 pos, Vec2 direction)
    {
        Command cmd(type);
        cmd.unit_id = unit_id;
        cmd.target_id = target_id;
        cmd.delta = delta;
        cmd.pos = pos;
        cmd.direction = direction;
        commands.push_back(cmd);
    }
    std::string getCommandTypeName(CommandType);
    std::string getActionTypeName(ActionType);

    bool plcomp(const Player& x, const Player& y);
};

#endif  //LOGIC_H