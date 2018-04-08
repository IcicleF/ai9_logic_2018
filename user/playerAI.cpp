//
// A simple sample AI.
//

#include "sdk/sdk.h"

extern "C"
{

//Standard AI interface (called once per round in 'invokeAI')
AI_API void playerAI(const PlayerSight sight, Actions* actions)
{
    static FocusAttackStrategy focusAttackStrategy;
    static PatrolStrategy patrolStrategy;
    //static BlindItemStrategy blindItemStrategy;

    focusAttackStrategy.generateActions(sight, actions);
    patrolStrategy.generateActions(sight, actions);
    //blindItemStrategy.generateActions(sight, actions);
}

}
