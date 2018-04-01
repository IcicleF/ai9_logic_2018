//
// Created by IcyF on 2018/2/7.
//

#ifndef UINTERFACE_H
#define UINTERFACE_H

#include <string>
#include <cstdio>
#include <map>

#include "../dllinterface.h"
#include "../sdk/defs.h"
#include "../sdk/actions.h"
#include "../logic.h"

typedef DllInterface<const PlayerSight, Actions*> AInterface;

class UInterface
{
public:
    ~UInterface();
    static UInterface* getInstance();

    void init(int);
    void getPlayerIDs(int*);
    void setPlayerName(int, const char*);
    bool loadAI(const char*, int);
    bool invokeAI();
    void run();
    void getCommands(char*);
    int checkWin(bool);
    void getRank(int*);
    void setReplayFile(const char*);
    void closeReplayFile();

private:
    explicit UInterface();
    GameLogic logic;

    int mapInnerID(int);

    int n;
    std::vector<int> ids;
    std::map<int, std::string> names;
    std::vector<AInterface*> dlls;

    std::string jsoncmd;
    FILE *fout;
};

#endif //UINTERFACE_H
