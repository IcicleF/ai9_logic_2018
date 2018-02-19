//
// Created by IcyF on 2018/2/19.
//

#ifndef FOCUS_ATTACK_STRATEGY_H
#define FOCUS_ATTACK_STRATEGY_H

#include "strategy.h"

class FocusAttackStrategy : public Strategy
{
public:
    FocusAttackStrategy(int _target) : Strategy(), target(_target) { }
    virtual ~FocusAttackStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Offensive; }

private:
    int target;
};

#endif //FOCUS_ATTACK_STRATEGY_H
