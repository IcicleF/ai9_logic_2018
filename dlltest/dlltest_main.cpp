//
// Created by IcyF on 2018/2/7.
//

/* Purpose: Dll API Test*/

#include "ui/uiapi.h"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

#if defined(_MSC_VER)
const char dllpath[] = "../ai/Debug/emptyAI.dll";
#else
const char dllpath[] = "ai/libemptyAI.dll";
#endif

int main(int argc, char** argv)
{
    setReplayFile("output.txt");
    init(4);

    int *id = new int[4];
    getPlayerIDs(id);

    cout << "[dlltest_main] player id fetched." << endl;

	for (int i = 0; i < 4; ++i)
		if (!loadAI(dllpath, id[i]))
        {
            cout << "[dlltest_main] failed to load libemptyAI.dll..." << endl;
            return 0;
        }

    cout << "[dlltest_main] libemptyAI.dll loaded!" << endl;

    auto stime = system_clock::now();

    int rounds = 900;
    for (int i = 0; i < rounds; ++i)
    {
        invokeAI();
        run();
        if (checkWin() != -1)
            break;
    }

    auto etime = system_clock::now();

    cout << "[dlltest_main] round simulation finished." << endl;

    closeReplayFile();

    auto duration = duration_cast<milliseconds>(etime - stime);
    cout << "Time cost: " << double(duration.count()) << "ms" << endl;

    delete[] id;
    return 0;
}