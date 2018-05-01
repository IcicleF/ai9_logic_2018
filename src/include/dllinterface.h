//
// Created by IcyF on 2018/2/6.
// Now fully linux-ized.
//

#ifndef DLLINTERFACE_H
#define DLLINTERFACE_H

#include "sdk/actions.h"

#include <dlfcn.h>
#include <vector>

class DllInterface
{
public:
    using AIFunction = void(*)(const PlayerSight&, Actions*);

    DllInterface() : n(0), shared(nullptr) { }
    ~DllInterface()
    {
        for (auto dll: dlls)
            dlclose(dll);
    }
    void resize(int _n)
    {
        n = _n;
        dlls.resize(n);
        ais.resize(n);
        pids.resize(n);
        psight.resize(n);
    }
    bool load(int ind, const char* fileName)
    {
        dlls[ind] = dlopen(fileName, RTLD_NOW);
        if (!dlls[ind])
            return false;
        ais[ind] = reinterpret_cast<AIFunction>(dlsym(dlls[ind], "playerAI"));
        return (bool)(ais[ind]);
    }
    void startProcess();
    void getCommands(std::vector<Actions>&);
    void endProcess();

    int n;
    std::vector<PlayerSight> psight;

private:
    std::vector<void*> dlls;
    std::vector<AIFunction> ais;

    static const int MAX_PL = 8;
    static const int BUF_SIZE = 128 * 1024;
    struct shared_mem
    {
        int trigger, shutdown;
        int written[MAX_PL];
        char sight[MAX_PL][BUF_SIZE];
        char ans[MAX_PL][BUF_SIZE];
    };
    shared_mem *shared;
    int shm_id;
    char* shm_addr;
    std::vector<pid_t> pids;

};

#endif //DLLINTERFACE_H
