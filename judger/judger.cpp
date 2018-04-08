//
// Created by IcyF on 2018/3/25.
//

#include "ui/uiapi.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

const int MAXPLAYERNUM = 6;

int main(int argc, char** argv)
{
    map<string, string> parm;
    map<int, string> name, aipath;

    if (argc == 1)
    {
        cout << "AI9 2018 Judger" << endl << endl;
        cout << "Parameters：" << endl;
        cout << "-players=M      Set the number of players to M." << endl;
        cout << "-rounds=R       Set the maximum round to R (optional, default is 3000)." << endl;
        cout << "-replay=F       Set the replay file name to F (optional, default is replay.zip)." << endl;
        cout << "-nameX=N        Set the name of player #X to N (optional, default is \"Attendant\")." << endl;
        cout << "-pathX=P        Set the AI path of player #X to P." << endl;
        cout << endl;
        cout << "The range of X is from 0 to M-1." << endl;
        cout << "Unrecognized parameters will be ignored." << endl;
        cout << endl;
#if defined(_WIN32)
        cout << "Example. ./judger -players=3 -rounds=2000 -path0=ai0.dll -path1=\"ai0.dll\" -path2=\"ai/ai0.dll\"" << endl;
#elif defined(__APPLE__)
        cout << "Example. ./judger -players=3 -rounds=2000 -path0=ai0.dylib -path1=\"ai0.dylib\" -path2=\"ai/ai0.dylib\"" << endl;
#else
        cout << "Example. ./judger -players=3 -rounds=2000 -path0=ai0.so -path1=\"ai0.so\" -path2=\"ai/ai0.so\"" << endl;
#endif
        return 0;
    }

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
        cout << "Player count not given!" << endl;
        return 1;
    }
    int playerNum = stoi(parm["players"]);
    if (playerNum < 2 || playerNum > 8)
    {
        cout << "Player count out of range!" << endl;
        return 1;
    }

    int maxRounds = 660;
    if (parm.find("rounds") != parm.end())
        maxRounds = stoi(parm["rounds"]);

    string replayFile("replay.zip");
    if (parm.find("replay") != parm.end())
        replayFile = parm["replay"];

    setReplayFile(replayFile.c_str());

    init(playerNum);
    int id[MAXPLAYERNUM + 5] = {0};
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

    int rank[MAXPLAYERNUM + 5] = {0};
    getRank(rank);

    for (int i = 0; i < playerNum; ++i)
        for (int j = 0; j < playerNum; ++j)
            if (rank[i] == id[j])
            {
                cout << j << " ";
                break;
            }
    cout << endl;

    closeReplayFile();
    return 0;
}