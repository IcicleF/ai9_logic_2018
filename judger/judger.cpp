//
// Created by IcyF on 2018/3/25.
//

#include "ui/uiapi.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    map<string, string> parm;
    map<int, string> name, aipath;

    for (int i = 1; i < argc; ++i)
    {
        string s(argv[i]);
        size_t pos = s.find('=');
        if (s.length() == 0 || s[0] != '-' || pos == string::npos)
        {
            cout << "Parameter format incorrect!" << endl;
            return 1;
        }
        parm[s.substr(1, pos - 1)] = s.substr(pos + 1);
    }

    if (parm.find("players") == parm.end())
    {
        cout << "Players count not given!" << endl;
        return 1;
    }
    int playerNum = stoi(parm["players"]);

    int maxRounds = 5000;
    if (parm.find("rounds") != parm.end())
        maxRounds = stoi(parm["rounds"]);

    string replayFile("replay.json");
    if (parm.find("replay") != parm.end())
        replayFile = parm["replay"];

    setReplayFile(replayFile.c_str());

    init(playerNum);
    int id[playerNum] = {0};
    getPlayerIDs(id);

    for (int i = 0; i < playerNum; ++i)
    {
        string pname = "name" + to_string(i);
        string ppath = "path" + to_string(i);
        if (parm.find(ppath) == parm.end())
        {
            cout << "AI path of player #" << i << " not specified!" << endl;
            return 1;
        }
        if (parm.find(pname) == parm.end())
            parm[pname] = "Attendant";
        name[id[i]] = parm[pname];
        aipath[id[i]] = parm[ppath];

        setPlayerName(id[i], name[id[i]].c_str());
    }

    for (int i = 0; i < playerNum; ++i)
        if (!loadAI(aipath[id[i]].c_str(), id[i]))
        {
            cout << "Failed to load " << aipath[id[i]] << "!" << endl;
            return 1;
        }

    for (int i = 0; i < maxRounds; ++i)
    {
        invokeAI();
        run();
        if (checkWin() != -1)
            break;
    }

    int winner = checkWin(true);
    for (int i = 0; i < playerNum; ++i)
        if (id[i] == winner)
        {
            cout << i << endl;
            break;
        }

    closeReplayFile();
    return 0;
}