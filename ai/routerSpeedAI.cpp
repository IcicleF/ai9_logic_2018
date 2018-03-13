//
// Created by IcyF on 2018/2/9.
//

#include "sdk/sdk.h"
#include "sdk/strategy/blind_item_strategy.h"
#include "sdk/strategy/stealth_strategy.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
__declspec(dllexport) void playerAI(const PlayerSight sight, Actions* actions)
{
    if (sight.round == 1)
    {
        SDK::actionMaker()->addStrategy<StealthStrategy>("RandAttack", 100);
        SDK::actionMaker()->addStrategy<BlindItemStrategy>("BlindItem");
    }
    SDK::actionMaker()->make(sight, actions);
}

}
