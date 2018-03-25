//
// Created by IcyF on 2018/2/7.
//

/* Purpose: Dll API Test*/

#include "ui/uiapi.h"
#include <iostream>
#include <cstring>
#include <chrono>

using namespace std;
using namespace chrono;

#define NUM 4

#if defined(_MSC_VER)
const char dllpath[] = "../ai/Debug/routerSpeedAI.dll";
#else
const char dllpath[] = "ai/librouterSpeedAI.dll";
#endif

int main(int argc, char** argv)
{
    setReplayFile("output.txt");
    init(NUM);

    int *id = new int[NUM];
    getPlayerIDs(id);

    for (int i = 0; i < NUM; ++i)
    {
        char s[15] = "ATTENDANT #X";
        s[11] = '0' + i;
        setPlayerName(id[i], s);
    }

    cout << "[dlltest_main] player id fetched." << endl;

	for (int i = 0; i < NUM; ++i)
		if (!loadAI(dllpath, id[i]))
        {
            cout << "[dlltest_main] failed to load " << dllpath << "..." << endl;
            return 0;
        }

    cout << "[dlltest_main] " << dllpath << " loaded!" << endl;

    auto stime = system_clock::now();

    int rounds = 600;
    if (argc > 1)
        rounds = atoi(argv[1]);
    for (int i = 0; i < rounds; ++i)
    {
        //cout << "round #" << i << ": start." << endl;
        invokeAI();
        //cout << "round #" << i << ": ai invoked." << endl;
        run();
        //cout << "round #" << i << ": round calced." << endl;
        if (checkWin() != -1)
            break;
    }

    int winner = checkWin(true);
    cout << "[dlltest_main] winner = " << winner << endl;

    auto etime = system_clock::now();

    cout << "[dlltest_main] round simulation finished." << endl;

    closeReplayFile();

    auto duration = duration_cast<milliseconds>(etime - stime);
    cout << "Time cost: " << double(duration.count()) << "ms" << endl;

    delete[] id;
    return 0;
}