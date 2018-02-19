#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include "sdk/defs.h"

enum UnitType
{
    VillagerType,
    PlayerType,
    UnitTypeCount
};

class GameLogic;
struct Unit
{
    int id;
    int hp;
    Vec2 position, velocity;

    bool moved;
    int currentTarget;
    std::vector<Vec2> targets;

    int waitUntil;
    int respawnWhen;

    GameLogic *logic;

    Unit(GameLogic *caller);
    virtual UnitType getUnitType() { return VillagerType; }
    virtual void getAction();

    virtual void receiveDeathPunishment() { }
    virtual void clearContinuousKill() { }
    virtual bool canUseBomb() { return false; }
    virtual bool canUseWard() { return false; }
    virtual bool hasBomb() { return false; }
    virtual bool hasWard() { return false; }
    virtual void useBomb() { }
    virtual void useWard() { }
    virtual bool canBuyBomb() { return false; }
    virtual bool canBuyWard() { return false; }
    virtual void buyBomb() { }
    virtual void buyWard() { }
    virtual bool canSuckAttack() { return false; }
    virtual void suckAttack() { }
    virtual void refresh() { }

    virtual ~Unit() { }
};

struct Player : public Unit
{
    int gold;
    int score;
    int bombCount, wardCount;
    int bombCD, wardCD, suckAttackCD;
    int continuousKill;
    int killedEnemy, killedVillager;

    Player(GameLogic *caller) : Unit(caller), gold(0), score(0), bombCount(0), wardCount(0),
        bombCD(0), wardCD(0), suckAttackCD(0),
        continuousKill(0), killedEnemy(0), killedVillager(0) { }
    virtual UnitType getUnitType() { return PlayerType; }

    virtual void receiveDeathPunishment()
    {
        bombCD = wardCD = suckAttackCD = 0;
        gold = int(gold * DeathPunishment);
    }
    virtual void clearContinuousKill() { continuousKill = 0; }
    virtual bool canUseBomb() { return hp > 0 && bombCD == 0; }
    virtual bool canUseWard() { return hp > 0 && wardCD == 0; }
    virtual bool hasBomb() { return bombCount > 0; }
    virtual bool hasWard() { return wardCount > 0; }
    virtual void useBomb() { if (hp == 0 || bombCount == 0 || bombCD > 0) return; --bombCount; bombCD = BombCD; }
    virtual void useWard() { if (hp == 0 || wardCount == 0 || wardCD > 0) return; --wardCount; wardCD = WardCD; }
    virtual bool canBuyBomb() { return hp > 0 && gold >= BombPrice; }
    virtual bool canBuyWard() { return hp > 0 && gold >= WardPrice; }
    virtual void buyBomb() { if (hp == 0) return; gold -= BombPrice; ++bombCount; }
    virtual void buyWard() { if (hp == 0) return; gold -= WardPrice; ++wardCount; }
    virtual bool canSuckAttack() { return hp > 0 && suckAttackCD == 0; }
    virtual void suckAttack() { if (hp == 0) return; suckAttackCD = SuckAttackCD; }
    virtual void refresh()
    {
        if (bombCD > 0) --bombCD;
        if (wardCD > 0) --wardCD;
        if (suckAttackCD > 0) --suckAttackCD;
    }

    virtual ~Player() { }
};

#endif  //UNIT_H