#include "sdk/sdk.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
__declspec(dllexport) void playerAI(const PlayerSight sight, Actions* actions)
{
    //do nothing
}

}