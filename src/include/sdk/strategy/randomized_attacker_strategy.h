//
// Created by IcyF on 2018/2/18.
//

#ifndef RANDOMIZED_ATTACKER_STRATEGY_H
#define RANDOMIZED_ATTACKER_STRATEGY_H

#include "strategy.h"
#include <set>

class RandomizedAttackerStrategy : public Strategy
{
public:
    RandomizedAttackerStrategy() : Strategy() { }
    virtual ~RandomizedAttackerStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Offensive; }

private:
    /**
     * attacked记录被攻击过的角色，已经攻击过的则不再攻击。
     * 这主要是为了防止击杀单位导致自身暴露。
     */
    std::set<int> attacked;
};

#endif //RANDOMIZED_ATTACKER_STRATEGY_H
