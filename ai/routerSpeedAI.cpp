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
    static RandomizedAttackerStrategy attackerStrategy;
    static PatrolStrategy patrolStrategy;
    static BlindItemStrategy blindItemStrategy;

    attackerStrategy.generateActions(sight, actions);
    patrolStrategy.generateActions(sight, actions);
    blindItemStrategy.generateActions(sight, actions);
}

}
