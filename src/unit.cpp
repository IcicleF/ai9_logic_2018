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

    if (this->getUnitType() != VillagerType)
        return;
    int hajime = Randomizer::getInstance()->randWaitTime() - (WanderIntervalLB + WanderIntervalUB) / 2;
    if (hajime > 0)
        waitUntil = caller->getCurrentRound() + hajime + 1;
}
void Unit::getAction()
{
    /**
     * todo
     * 要对玩家“友好”。现在的情况是，村民漫无目的地乱走，这情况对玩家来说可能比较困难。
     * 因此，考虑在村民的移动中，随机地加入一些确定性的动作（走平行坐标轴直线？目前还没想好）
     * 通过这种手段，有意识地将玩家的关注点转移到这些确定性的动作上，诱导他们主动通过这些动作进行模式识别，同时伪装自己。
     */
    Actions act;
    if (logic->getCurrentRound() >= waitUntil)
    {
        waitUntil = IINF;
        act.emplace(SelectDestination, -1, Router::getInstance()->availablePosition());
    }
    else if (currentTarget == -1)
        waitUntil = logic->getCurrentRound() + Randomizer::getInstance()->randWaitTime();
    logic->reportActions(this->id, act);
}

void Player::receiveDeathPunishment()
{
    bombCD = wardCD = suckAttackCD = 0;
    int gold_n = int(gold * DeathPunishment);
    logic->addCommand(GoldChange, id, gold_n - gold);
    gold = gold_n;
}