//
// Created by IcyF on 2018/2/19.
//

#include "patrol_strategy.h"
#include "sdk/randomizer.h"

using namespace std;

void PatrolStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    if (sight.velocity == Vec2())
    {
        if (status == psWalking)    //开始等待
        {
            status = psStandby;
            activeRound = sight.round + Randomizer::getInstance()->randWaitTime();
        }
        else if (status == psStandby && sight.round == activeRound)
        {
            status = psWalking;
            activeRound = 0;
            currentTarget = (currentTarget + 1) % 4;
            Vec2 goal = Randomizer::getInstance()->randPosition() * 4 / MapSize;
            switch (currentTarget)
            {
                case 0:  //LD
                    goal = goal + Vec2(-MapSize + 6, -MapSize + 6);
                    break;
                case 1:  //LU
                    goal = goal + Vec2(-MapSize + 6, MapSize - 6);
                    break;
                case 2:  //RU
                    goal = goal + Vec2(MapSize - 6, -MapSize - 6);
                    break;
                case 3:  //RD
                    goal = goal + Vec2(MapSize - 6, -MapSize + 6);
                    break;
                default: //Illegal, ought not happen
                    goal = sight.pos;
            }
            actions->emplace(SelectDestination, sight.id, goal);
        }
    }
}