//
// Created by IcyF on 2018/2/17.
//

#ifndef STRATEGY_H
#define STRATEGY_H

#include "sdk/actions.h"

enum StrategyType
{
    Defensive,
    Neutral,
    Offensive
};

class ActionMaker;
class Strategy
{
    friend class ActionMaker;
public:
    Strategy() { magic = MAGIC; }
    virtual ~Strategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*) = 0;
    virtual StrategyType getStrategyType() { return Neutral; }

protected:
    unsigned int magic;
};

#endif //STRATEGY_H
