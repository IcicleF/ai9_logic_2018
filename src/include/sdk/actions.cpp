//
// Created by IcyF on 2018/2/8.
//

#include "actions.h"

void Actions::emplace(ActionType type, int target_id, Vec2 pos)
{
    Action act(type);
    act.target_id = target_id;
    act.pos = pos;
    actions.push_back(act);
}