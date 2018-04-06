//
// Created by IcyF on 2018/2/6.
//

#ifndef DLLINTERFACE_H
#define DLLINTERFACE_H

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "sdk/actions.h"

class DllInterface
{
public:
    using AIFunction = void(*)(const PlayerSight&, Actions*);

    DllInterface() = default;
    ~DllInterface()
    {
#ifdef _WIN32
        FreeLibrary(dll);
#else
        dlclose(dll);
#endif
    }
    bool load(const char* fileName)
    {
#ifdef _WIN32
        dll = LoadLibrary(fileName);
    if (!dll)
        return false;
    ai = (AIFunction)GetProcAddress(dll, "playerAI");
    if (!ai)
        return false;
#else
        dll = dlopen(fileName, RTLD_NOW);
        if (!dll)
            return false;
        ai = reinterpret_cast<AIFunction>(dlsym(dll, "playerAI"));
        if (!ai)
            return false;
#endif
        return true;
    }

    bool getCommands(const PlayerSight&, Actions*);

private:
#ifdef _WIN32
    HMODULE dll;
#else
    void* dll;
#endif
    AIFunction ai;

public:
    int playerID;
};

#endif //DLLINTERFACE_H
