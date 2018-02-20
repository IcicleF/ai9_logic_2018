//
// Created by IcyF on 2018/2/17.
//

#include "actionmaker.h"

using namespace std;

ActionMaker::~ActionMaker()
{
    removeAll();
}

template <typename Tp, typename... Types>
void ActionMaker::addStrategy(string name, Types... args)
{
    Tp* tp = new Tp(args...);
    Strategy* s = reinterpret_cast<Strategy*>(tp);
    if (s->magic != MAGIC)
    {
        delete tp;
        return;
    }
    removeStrategy(name);
    strategies[name] = s;
}

void ActionMaker::removeStrategy(string name)
{
    auto it = strategies.find(name);
    if (it != strategies.end())
        strategies.erase(it);
}
void ActionMaker::removeStrategies(StrategyType type)
{
    for (auto it = strategies.begin(); it != strategies.end(); )
        if (it->second->getStrategyType() == type)
            it = strategies.erase(it);
        else
            ++it;
}
void ActionMaker::removeAll()
{
    for (auto it = strategies.begin(); it != strategies.end(); ++it)
        delete it->second;
    strategies.clear();
}

void ActionMaker::make(const PlayerSight &sight, Actions *actions)
{
    if (actions == nullptr)
        return;
    for (auto it = strategies.begin(); it != strategies.end(); ++it)
        if (it->second != nullptr)
            it->second->generateActions(sight, actions);
}