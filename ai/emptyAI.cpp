#include "sdk/sdk.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
AI_API void playerAI(const PlayerSight sight, Actions* actions)
{
    //do nothing
}

}