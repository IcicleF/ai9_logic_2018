//
// Created by IcyF on 2018/2/17.
//

#ifndef ACTIONMAKER_H
#define ACTIONMAKER_H

#include "strategy.h"
#include "sdk/defs.h"

#include <map>
#include <string>

class ActionMaker
{
public:
    explicit ActionMaker(PlayerSight* _sight = nullptr, Actions* _actions = nullptr)
            : sight(_sight), actions(_actions) { }
    ~ActionMaker();

    template <typename Tp, typename... Types>
    void addStrategy(std::string, Types...);
    void removeStrategy(std::string);
    void removeStrategies(StrategyType);

    void make();

private:
    Actions* actions;
    PlayerSight* sight;
    std::map<std::string, Strategy*> strategies;
};

#endif //ACTIONMAKER_H
