//
// Created by IcyF on 2018/2/9.
//

#include "sdk/sdk.h"
#include "sdk/strategy/blind_item_strategy.h"
#include "sdk/strategy/randomized_attacker_strategy.h"
#include "sdk/strategy/patrol_strategy.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
AI_API void playerAI(const PlayerSight sight, Actions* actions)
{
    if (sight.round == 1)
    {
        SDK::actionMaker()->addStrategy<RandomizedAttackerStrategy>("RandAttack");
        SDK::actionMaker()->addStrategy<PatrolStrategy>("Patrol");
        SDK::actionMaker()->addStrategy<BlindItemStrategy>("BlindItem");
    }
    SDK::actionMaker()->make(sight, actions);
}

}
