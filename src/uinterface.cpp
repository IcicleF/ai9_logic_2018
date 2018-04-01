//
// Created by IcyF on 2018/2/7.
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "ui/uinterface.h"
#include "json/json.h"

using namespace std;

UInterface::UInterface()
{
    fout = nullptr;
}
UInterface::~UInterface()
{
    for (auto dll : dlls)
        if (dll != nullptr)
            delete dll;
}

UInterface* UInterface::getInstance()
{
    static UInterface instance;
    return &instance;
}
void UInterface::setReplayFile(const char* rf)
{
    fout = fopen(rf, "w");
    if (!fout)
        fout = nullptr;
    fprintf(fout, "{\"gameinfo\":[");
}
void UInterface::init(int playerCount)
{
    n = playerCount;
    logic.startGame(n);
    jsoncmd = logic.getCommands();
    if (fout == nullptr)
        return;
    fprintf(fout, "%s", jsoncmd.c_str());

    ids.assign(n, -1);
    dlls.assign(n, nullptr);
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

    dlls[ind] = new AInterface;
    dlls[ind]->playerID = playerID;
    bool res = dlls[ind]->load(aiPath);

    if (!res)
    {
        delete dlls[ind];
        dlls[ind] = nullptr;
    }
    return res;
}
bool UInterface::invokeAI()
{
    for (int i = 0; i < n; ++i)
        if (dlls[i] == nullptr)
            return false;
    logic.startReporting();
    logic.preCalc();
    for (int i = 0; i < n; ++i)
    {
        Actions acts;
        dlls[i]->getCommands(logic.getSight(ids[i]), &acts);
        logic.reportActions(ids[i], acts);
    }
    return true;
}
void UInterface::run()
{
    logic.calcRound();
    jsoncmd = logic.getCommands();
    if (fout == nullptr)
        return;
    fprintf(fout, ", %s", jsoncmd.c_str());
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
void UInterface::getRank(int* playerRank)
{
    logic.getRank(playerRank);
}
void UInterface::closeReplayFile()
{
    if (fout == nullptr)
        return;
	fprintf(fout, "],\"participants\":%d,\"rounds\":%d,", logic.getPlayerCount(), logic.getCurrentRound());
    Json::Value jn;
    for (auto id : ids)
        jn[to_string(id)] = names[id];
    Json::FastWriter fw;
    fprintf(fout, "\"names\":%s}", fw.write(jn).c_str());
    fclose(fout);
    fout = nullptr;
}

int UInterface::mapInnerID(int logicID)
{
    for (int i = 0; i < ids.size(); ++i)
        if (ids[i] == logicID)
            return i;
    return -1;
}