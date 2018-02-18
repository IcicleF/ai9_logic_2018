//
// Created by IcyF on 2018/2/9.
//

#include <chrono>
#include "randomizer.h"

using namespace std;

Randomizer* Randomizer::getInstance()
{
    static Randomizer instance;
    return &instance;
}
Randomizer::Randomizer()
{
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    RndCore = new mt19937(seed);
    AxisRnd = new uniform_real_distribution<float>(-MapSize, MapSize);
    RespawnRnd = new uniform_real_distribution<float>(-1, 49);
    WanderRnd = new uniform_int_distribution<int>(WanderIntervalLB, WanderIntervalUB);
}
Randomizer::~Randomizer()
{
    delete RndCore;
    delete AxisRnd;
    delete RespawnRnd;
    delete WanderRnd;
}

float Randomizer::randAxis()
{
    return (*AxisRnd)(*RndCore);
}
Vec2 Randomizer::randPosition()
{
    return Vec2(randAxis(), randAxis());
}
bool Randomizer::randSpawnJudger()
{
    return (*RespawnRnd)(*RndCore) < 0;
}
int Randomizer::randWaitTime()
{
    return (*WanderRnd)(*RndCore);
}