//
// Created by IcyF on 2018/2/7.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "ui/uinterface.h"
#include "json/json.h"
#include "miniz/miniz.h"

using namespace std;

UInterface* UInterface::getInstance()
{
    static UInterface instance;
    return &instance;
}
void UInterface::setReplayFile(const char* rf)
{
    outfile = string(rf);
    full_json = "{\"gameinfo\":[";
}
void UInterface::init(int playerCount)
{
    n = playerCount;
    logic.startGame(n);
    jsoncmd = logic.getCommands();
    full_json += jsoncmd;

    ids.assign(n, -1);
    dlls.resize(n);
}
void UInterface::getPlayerIDs(int* playerIDs)
{
    auto ivec = logic.getIDs();
    for (int i = 0; i < ivec.size(); ++i)
        playerIDs[i] = ids[i] = ivec[i];
}
void UInterface::setPlayerName(int playerID, const char* name)
{
    names[playerID] = string(name);
}
bool UInterface::loadAI(const char* aiPath, int playerID)
{
    int ind = mapInnerID(playerID);
    if (ind == -1)
        return false;
    return dlls.load(ind, aiPath);
}
void UInterface::beginJudge()
{
    dlls.startProcess();
}
bool UInterface::invokeAI()
{
    logic.startReporting();
    logic.preCalc();

    for (auto id : ids)
        dlls.psight[mapInnerID(id)] = logic.getSight(id);
    vector<Actions> acts;
    dlls.getCommands(acts);
    for (int i = 0; i < n; ++i)
        logic.reportActions(ids[i], acts[i]);

    return true;
}
void UInterface::run()
{
    logic.calcRound();
    jsoncmd = logic.getCommands();
    full_json += ", " + jsoncmd;
}
void UInterface::getCommands(char* commandStr)
{
    if (commandStr == nullptr)
        return;
    strcpy(commandStr, jsoncmd.c_str());
}
int UInterface::checkWin(bool forced)
{
    return logic.judgeWin(forced);
}
void UInterface::endJudge()
{
    dlls.endProcess();
}
void UInterface::getRank(int* playerRank)
{
    logic.getRank(playerRank);
}
void UInterface::closeReplayFile()
{
    full_json += "],\"participants\":" + to_string(logic.getPlayerCount())
                 + ",\"rounds\":" + to_string(logic.getCurrentRound()) + ",";

    Json::Value jn;
    for (auto id : ids)
        jn[to_string(id)] = names[id];

    Json::FastWriter fw;
    full_json += "\"names\":" + fw.write(jn) + "}";

    //Compressing
    remove(outfile.c_str());
    int status = mz_zip_add_mem_to_archive_file_in_place(outfile.c_str(), "replay.json", full_json.c_str(), full_json.length(), "", 0, MZ_DEFAULT_LEVEL);
    if (!status)
        return;
    full_json.clear();
}

int UInterface::mapInnerID(int logicID)
{
    for (int i = 0; i < ids.size(); ++i)
        if (ids[i] == logicID)
            return i;
    return -1;
}