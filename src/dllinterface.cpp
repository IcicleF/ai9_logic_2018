//
// Created by icyf on 2018-4-6.
//

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

#include "dllinterface.h"
#include <iostream>
#include <chrono>

using namespace std;

using chrono::system_clock;
using chrono::milliseconds;

void DllInterface::startProcess()
{
    if (dlls.size() != n || n == 0)
        return;

    pid_t pid = -1;
    int cur = -1;

    if ((shm_id = shmget(IPC_PRIVATE, sizeof(shared_mem), IPC_CREAT | 0666)) < 0)
        exit(1);
    shm_addr = (char*)shmat(shm_id, NULL, 0);
    if (shm_addr == (void*)-1)
        exit(1);

    shared = (shared_mem*)shm_addr;
    for (int i = 0; i < n; ++i)
        shared->written[i] = 0;
    shared->trigger = shared->shutdown = 0;

    for (int i = 0; i < n; ++i)
    {
        pid = fork();
        if (pid == -1)
            exit(1);
        if (pid == 0)
        {
            cur = i;
            break;
        }
        else
            pids[i] = pid;
    }

    if (pid == 0)
    {
        freopen("/dev/null", "w", stdout);
        if (cur == -1)
            exit(1);
        while (shared->shutdown == 0)
        {
            if (shared->trigger == 1 && shared->written[cur] == 0)
            {
                PlayerSight ps;
                istringstream sin(shared->sight[cur]);

                //cout << shared->sight[cur] << endl << endl;
                
                sin >> ps.round >> ps.id >> ps.pos.x >> ps.pos.y 
                    >> ps.hp >> ps.velocity.x >> ps.velocity.y
                    >> ps.bombCount >> ps.wardCount >> ps.canUseBomb
                    >> ps.canUseWard >> ps.canSuckAttack >> ps.gold >> ps.score
                    >> ps.continuousKill;

                sin >> ps.placedWardCount;
                //cout << "    [subproc #" << cur << "] " << ps.placedWardCount;
                ps.placedWard.resize(ps.placedWardCount);
                for (int i = 0; i < ps.placedWardCount; ++i)
                    sin >> ps.placedWard[i].pos.x >> ps.placedWard[i].pos.y >> ps.placedWard[i].life;
                sin >> ps.bombInSightCount;
                //cout << " " << ps.bombInSightCount;
                ps.bombInSight.resize(ps.bombInSightCount);
                for (int i = 0; i < ps.bombInSightCount; ++i)
                    sin >> ps.bombInSight[i].pos.x >> ps.bombInSight[i].pos.y
                        >> ps.bombInSight[i].velocity.x >> ps.bombInSight[i].velocity.y;
                sin >> ps.unitInSightCount;
                //cout << " " << ps.unitInSightCount;
                ps.unitInSight.resize(ps.unitInSightCount);
                for (int i = 0; i < ps.unitInSightCount; ++i)
                    sin >> ps.unitInSight[i].id >> ps.unitInSight[i].pos.x >> ps.unitInSight[i].pos.y
                        >> ps.unitInSight[i].velocity.x >> ps.unitInSight[i].velocity.y;
                sin >> ps.corpseInSightCount;
                //cout << " " << ps.corpseInSightCount << endl;
                ps.corpseInSight.resize(ps.corpseInSightCount);
                for (int i = 0; i < ps.corpseInSightCount; ++i)
                    sin >> ps.corpseInSight[i].x >> ps.corpseInSight[i].y;
                ps.scoreBoard.resize(n);
                for (int i = 0; i < n; ++i)
                    sin >> ps.scoreBoard[i];

                //cout << "    [subproc #" << cur << "] sight received." << endl;

                Actions actions;
                (ais[cur])(ps, &actions);

                //cout << "    [subproc #" << cur << "] action got." << endl;

                ostringstream ss;
                ss.clear();
                ss << actions.actions.size() << " ";
                for (Action &act : actions.actions)
                    ss << (int) act.actionType << " "
                       << act.target_id << " "
                       << act.pos.x << " " << act.pos.y << " ";
                strcpy(shared->ans[cur], ss.str().c_str());
                shared->written[cur] = 1;
            }
        }
        shmdt(shm_addr);
    }
}

void DllInterface::getCommands(std::vector<Actions>& acts)
{
    if (shared == nullptr || n == 0)
        return;

    //Translate sight information
    for (int i = 0; i < n; ++i)
    {
        PlayerSight& ps = psight[i];

        ostringstream ss;
        ss.clear();

        ss << ps.round << " " << ps.id << " " << ps.pos.x << " " << ps.pos.y << " "
           << ps.hp << " " << ps.velocity.x << " " << ps.velocity.y << " "
           << ps.bombCount << " " << ps.wardCount << " " << ps.canUseBomb << " "
           << ps.canUseWard << " " << ps.canSuckAttack << " " << ps.gold << " " << ps.score << " "
           << ps.continuousKill << endl;
        ss << ps.placedWardCount;
        for (auto pw : ps.placedWard)
            ss << " " << pw.pos.x << " " << pw.pos.y << " " << pw.life;
        ss << endl;
        ss << ps.bombInSightCount;
        for (auto pb : ps.bombInSight)
            ss << " " << pb.pos.x << " " << pb.pos.y << " " << pb.velocity.x << " " << pb.velocity.y;
        ss << endl;
        ss << ps.unitInSightCount;
        for (auto pu : ps.unitInSight)
            ss << " " << pu.id << " " << pu.pos.x << " " << pu.pos.y
               << " " << pu.velocity.x << " " << pu.velocity.y;
        ss << endl;
        ss << ps.corpseInSightCount;
        for (auto pc : ps.corpseInSight)
            ss << " " << pc.x << " " << pc.y;
        ss << endl;
        for (auto sc : ps.scoreBoard)
            ss << sc << " ";
        strcpy(shared->sight[i], ss.str().c_str());
    }

    //cout << "[major] sight translation finished." << endl;

    //Trigger sub-processes
    shared->trigger = 0;
    for (int i = 0; i < n; ++i)
        shared->written[i] = 0;
    shared->trigger = 1;

    auto start_time = chrono::system_clock::now();
    while (true)
    {
        bool flag = true;
        for (int i = 0; i < n; ++i)
        {
            kill(pids[i], 0);
            if (errno == ESRCH)
                continue;
            if (shared->written[i] != 1)
            {
                flag = false;
                break;
            }
        }
        if (flag)
            break;

        auto end_time = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        if (duration.count() > 150)
        {
            for (auto pid: pids)
                kill(pid, SIGKILL);
            break;
        }
        for (int i = 0; i < 1000; ++i);
    }

    //cout << "[major] subprocess work finished." << endl;

    acts.resize(n);
    for (int i = 0; i < n; ++i)
    {
        if (shared->written[i] != 1)
        {
            acts[i].actions.clear();
            continue;
        }
        istringstream ss(shared->ans[i]);
        int count = 0;
        ss >> count;

        acts[i].actions.resize(count);
        for (int j = 0; j < count; ++j)
        {
            unsigned int action_id;
            int target_id;
            float x, y;

            ss >> action_id >> target_id >> x >> y;
            acts[i].actions[j].actionType = (ActionType)action_id;
            acts[i].actions[j].target_id = target_id;
            acts[i].actions[j].pos = Vec2(x, y);
        }
    }
}

void DllInterface::endProcess()
{
    if (shared == nullptr)
        return;
    shared->shutdown = 1;
    for (int i = 1; i <= 100000; ++i);
    shmctl(shm_id, IPC_RMID, NULL);
}