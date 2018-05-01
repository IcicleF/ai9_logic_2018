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

class UInterface
{
public:
    ~UInterface() = default;
    static UInterface* getInstance();

    void init(int);
    void getPlayerIDs(int*);
    void setPlayerName(int, const char*);
    bool loadAI(const char*, int);
    void beginJudge();
    bool invokeAI();
    void run();
    void getCommands(char*);
    int checkWin(bool);
    void endJudge();
    void getRank(int*);
    void setReplayFile(const char*);
    void closeReplayFile();

private:
    explicit UInterface() = default;
    GameLogic logic;

    int mapInnerID(int);

    int n;
    std::vector<int> ids;
    std::map<int, std::string> names;
    DllInterface dlls;

    std::string jsoncmd;
    std::string full_json;
    std::string outfile;
};

#endif //UINTERFACE_H
