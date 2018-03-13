//
// Created by IcyF on 2018/2/6.
//

#ifndef DLLINTERFACE_H
#define DLLINTERFACE_H

#include <thread>
#include <atomic>
#include <chrono>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "sdk/defs.h"

template <typename... Types>
class DllInterface
{
public:
    using AIFunction = void(*)(Types... args);

    DllInterface() = default;
    ~DllInterface();

    bool load(const char* fileName);
    bool getCommands(Types... args);

    int playerID;

private:
#ifdef _WIN32
    HMODULE dll;
#else
    void* dll;
#endif
    AIFunction ai;
};

template <typename... Types>
bool DllInterface<Types...>::load(const char *fileName)
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
    ai = reinterpret_cast<AIFunction>(dlsym(dll, AIFuncName));
    if (!ai)
        return false;
#endif
    return true;
}

template <typename... Types>
bool DllInterface<Types...>::getCommands(Types... args)
{
    /*
    using namespace std::chrono;
    int ret = 0;
    auto run = [&ret, &args..., this]()
    {
        try
        {
            ai(args...);
        }
        catch (std::exception &e)
        {
            ret = -1;
            return;
        }
        ret = 1;
    };
    auto terminateTime = system_clock::now() + milliseconds(500);
	std::thread* aiThread = new std::thread(run);
    while (system_clock::now() < terminateTime && ret == 0);
    aiThread->detach();
	delete aiThread;
    */
    if (ai)
        ai(args...);
    return true;
}

template <typename... Types>
DllInterface<Types...>::~DllInterface()
{
#ifdef _WIN32
    FreeLibrary(dll);
#else
    dlclose(dll);
#endif
}

#endif //DLLINTERFACE_H
