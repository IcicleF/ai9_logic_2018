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
    explicit ActionMaker() { }
    ~ActionMaker();

    template <typename Tp, typename... Types>
    void addStrategy(std::string, Types...);
    void removeStrategy(std::string);
    void removeStrategies(StrategyType);
    void removeAll();

    void make(const PlayerSight&, Actions*);

private:
    std::map<std::string, Strategy*> strategies;
};

#endif //ACTIONMAKER_H
