//
// Created by IcyF on 2018/2/7.
//

#ifndef UINTERFACE_H
#define UINTERFACE_H

#include <string>
#include <cstdio>

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
    bool loadAI(const char*, int);
    bool invokeAI();
    void run();
    void getCommands(char*);
    int checkWin(bool);
    void setReplayFile(const char*);
    void closeReplayFile();

private:
    explicit UInterface();
    GameLogic logic;

    int mapInnerID(int);

    int n;
    std::vector<int> ids;
    std::vector<AInterface*> dlls;

    std::string jsoncmd;
    FILE *fout;
};

#endif //UINTERFACE_H
