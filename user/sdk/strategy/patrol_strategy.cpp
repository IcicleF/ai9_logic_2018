//
// Created by IcyF on 2018/2/19.
//

#include "patrol_strategy.h"
#include "../randomizer.h"

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
            currentTarget = (currentTarget + 1) % 5;
            Vec2 goal(Randomizer::getInstance()->randXAxis() * 4 / MapWidth,
                      Randomizer::getInstance()->randYAxis() * 4 / MapHeight);
            switch (currentTarget)
            {
                case 0:
                    goal = goal + Vec2(27, 82);
                    break;
                case 1:
                    goal = goal + Vec2(16, 46);
                    break;
                case 2:
                    goal = goal + Vec2(23, 7);
                    break;
                case 3:
                    goal = goal + Vec2(40, 55);
                    break;
                case 4:
                    goal = goal + Vec2(55, 84);
                    break;
                default:
                    goal = sight.pos;
            }
            actions->emplace(SelectDestination, sight.id, goal);
        }
    }
}