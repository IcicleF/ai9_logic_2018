//
// Created by IcyF on 2018/2/7.
//

#include "ui/uiapi.h"
#include "ui/uinterface.h"

void init(int playerCount)
{
    UInterface::getInstance()->init(playerCount);
}
void setReplayFile(const char* rf)
{
    UInterface::getInstance()->setReplayFile(rf);
}
void getPlayerIDs(int* playerIDs)
{
    UInterface::getInstance()->getPlayerIDs(playerIDs);
}
void setPlayerName(int playerID, const char* name)
{
    UInterface::getInstance()->setPlayerName(playerID, name);
}
bool loadAI(const char* aiPath, int playerID)
{
    return UInterface::getInstance()->loadAI(aiPath, playerID);
}
bool invokeAI()
{
    return UInterface::getInstance()->invokeAI();
}
void run()
{
    UInterface::getInstance()->run();
}
void getCommands(char* commandStr)
{
    UInterface::getInstance()->getCommands(commandStr);
}
int checkWin(bool forced)
{
    return UInterface::getInstance()->checkWin(forced);
}
void closeReplayFile()
{
    UInterface::getInstance()->closeReplayFile();
}