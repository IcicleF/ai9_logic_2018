//
// Created by icyf on 2018-4-6.
//

#define USE_WINDOWS_JUDGE

#ifndef USE_WINDOWS_JUDGE

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/types.h>
#include <csignal>

#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdio>

#include <chrono>

#else

#include <chrono>
#include <thread>
#include <atomic>

#endif

#include "dllinterface.h"
#include <iostream>

using namespace std;
using chrono::system_clock;
using chrono::milliseconds;

const int BUF_SIZE = 128 * 1024;
struct shared_mem
{
    int written;
    char buf[BUF_SIZE];
};

bool DllInterface::getCommands(const PlayerSight& sight, Actions* actions)
{
    if (!(ai)) return false;

#ifdef USE_WINDOWS_JUDGE
    int signal = 0;
    auto run = [&signal, &sight, actions, this]() {
        try {
            ai(sight, actions);
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            signal = -1;
            return;
        }
        signal = 1;
    };
    thread ai(run);
    auto terminateTime = system_clock::now() + milliseconds(150);
    while (system_clock::now() < terminateTime && signal == 0);
    ai.detach();
    return signal == 1;
#else
    int shm_id;
    char* shm_addr;

    pid_t pid;

    if ((shm_id = shmget(IPC_PRIVATE, sizeof(shared_mem), IPC_CREAT | 0666)) < 0)
        exit(1);
    shm_addr = (char*)shmat(shm_id, NULL, 0);
    if (shm_addr == (void*)-1)
        exit(1);

    shared_mem *shared = (shared_mem*)shm_addr;
    shared->written = 0;

    auto start_time = chrono::system_clock::now();

    if ((pid = fork()) == -1)
        exit(1);
    if (pid == 0)
    {
        freopen("/dev/null", "w", stdout);
        ai(sight, actions);

        ostringstream ss;
        ss.clear();
        ss << actions->actions.size() << " ";
        for (Action& act : actions->actions)
            ss << (int)act.actionType << " "
               << act.target_id << " "
               << act.pos.x << " " << act.pos.y << " ";
        strcpy(shared->buf, ss.str().c_str());
        shared->written = 1;
        shmdt(shm_addr);
        exit(0);
    }
    else
    {
        while (shared->written == 0)
        {
            auto end_time = chrono::system_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            if (duration.count() > 150)
            {
                kill(pid, SIGKILL);

                actions->actions.clear();
                shmdt(shm_addr);
                shmctl(shm_id, IPC_RMID, NULL);
                return true;
            }
            for (int i = 0; i < 1000; ++i);
        }

        istringstream ss(shared->buf);
        int count = 0;
        ss >> count;
        actions->actions.resize(count);
        for (int i = 0; i < count; ++i)
        {
            unsigned int action_id;
            int target_id;
            float x, y;

            ss >> action_id >> target_id >> x >> y;
            actions->actions[i].actionType = (ActionType)action_id;
            actions->actions[i].target_id = target_id;
            actions->actions[i].pos = Vec2(x, y);
        }
        shmdt(shm_addr);
        shmctl(shm_id, IPC_RMID, NULL);
    }
    return true;
#endif
}