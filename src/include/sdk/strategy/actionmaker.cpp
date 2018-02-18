//
// Created by IcyF on 2018/2/17.
//

#include "actionmaker.h"

using namespace std;

ActionMaker::~ActionMaker()
{
    for (auto s : strategies)
        delete s;
}

template <typename Tp, typename... Types>
void ActionMaker::addStrategy(Types... args)
{
    Tp* tp = new Tp(args...);
    Strategy* s = reinterpret_cast<Strategy*>(tp);
    if (s->magic != MAGIC)
    {
        delete tp;
        return;
    }
    strategies.push_back(tp);
}

void ActionMaker::make()
{
    if (sight == nullptr || actions == nullptr)
        return;
    for (auto s : strategies)
        s->generateActions(*sight, actions);
}