//
// Created by IcyF on 2018/2/9.
//

//This is an AI aimed to test the router.
//This AI will call router once per round and drive the user crazy. OvO

#include "sdk/sdk.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
__declspec(dllexport) void playerAI(const PlayerSight sight, Actions* actions)
{
    actions->emplace(SelectDestination, -1, Randomizer::getInstance()->randPosition());
}

}