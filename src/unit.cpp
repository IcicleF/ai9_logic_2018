#include "unit.h"
#include "logic.h"

using namespace std;

//class Unit: 村民的行动
Unit::Unit(GameLogic *caller)
{
    logic = caller;
    hp = PlayerHP;
    respawnWhen = -1;
    currentTarget = -1;
    position = Router::getInstance()->availablePosition();
    stat = uNone;
    waitUntil = 0;

    if (this->getUnitType() != VillagerType)
        return;

    int hajime = Randomizer::getInstance()->randWaitTime() - (WanderIntervalLB + WanderIntervalUB) / 2;
    if (hajime > 0)
    {
        stat = uWaiting;
        waitUntil = caller->getCurrentRound() + hajime + 1;
    }
}
void Unit::getAction()
{
    Actions act;
    if (currentTarget != -1 && stat == uMoving)
        return;
    else if (stat == uMoving)
    {
        waitUntil = logic->getCurrentRound() + Randomizer::getInstance()->randWaitTime();
        stat = uWaiting;
    }
    else if (logic->getCurrentRound() >= waitUntil)
    {
        waitUntil = IINF;
        act.emplace(SelectDestination, -1, Router::getInstance()->availablePosition());
        stat = uMoving;
    }
    logic->reportActions(this->id, act);
}

void Player::receiveDeathPunishment()
{
    bombCD = wardCD = suckAttackCD = 0;
    int gold_n = int(gold * DeathPunishment);
    logic->addCommand(GoldChange, id, gold_n - gold);
    gold = gold_n;
}