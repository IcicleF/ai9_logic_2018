//
// Created by IcyF on 2018/2/19.
//

#include "focus_attack_strategy.h"

using namespace std;

void FocusAttackStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    for (auto uinfo : sight.unitInSight)
        if (uinfo.id == target)
        {
            if (sight.canSuckAttack && (uinfo.pos - sight.pos).length() <= SuckRange)
                actions->emplace(SuckAttack, uinfo.id, uinfo.pos);
            if (sight.canUseBomb)
            {
                if (sight.bombCount == 0)
                    actions->emplace(BuyItem, BombItem, Vec2());
                actions->emplace(UseItem, BombItem, uinfo.pos + uinfo.velocity);   //预测一回合行动
            }
        }
}