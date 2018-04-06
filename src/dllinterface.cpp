//
// Created by icyf on 2018-4-6.
//

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>

#include "dllinterface.h"

using namespace std;

const int SIZE = 64 * 1024;
struct shared_mem
{
    int written;
    char buf[SIZE];
};

bool DllInterface::getCommands(const PlayerSight& sight, Actions* actions)
{
    if (!(ai)) return false;
#ifdef _WIN32
    ai(args...);
    return true;
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
    if ((pid = fork()) == -1)
        exit(1);
    if (pid == 0)
    {
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
            for (int i = 0; i < 100000; ++i);

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
    }
    return true;
#endif
}