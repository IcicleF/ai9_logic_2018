//
// Created by IcyF on 2018/2/17.
//

#ifndef ACTIONMAKER_H
#define ACTIONMAKER_H

#include "strategy.h"
#include "../defs.h"

#include <map>
#include <string>

class ActionMaker
{
public:
    explicit ActionMaker() { }
    ~ActionMaker();

    template <typename Tp, typename... Types>
    void addStrategy(std::string name, Types... args)
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
	
    void removeStrategy(std::string);
    void removeStrategies(StrategyType);
    void removeAll();

    void make(const PlayerSight&, Actions*);

private:
    std::map<std::string, Strategy*> strategies;
};

#endif //ACTIONMAKER_H
