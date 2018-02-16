#ifndef LITERALS_H
#define LITERALS_H

const float PI = 3.1415926536;
const float EPS = 1e-6;
const int IINF = 99999999;
const float FINF = 99999999.0f;

const int JSONCMD_SIZE = 1200000;

/*
 *  内部回合
 */
const float InnerRoundLength = 100;             //每个内部回合的长度（毫秒）

/*
 *  地图信息
 */
const float MapSize = 40.0;                     //正方形地图，每边均是[-MapSize, MapSize]
const float ObsBorder = 0.05;                    //障碍物边界向外延伸的不可达区域宽度

/*
 *  玩家和村民
 */
const int PlayerHP = 100;                       //玩家出生时HP
const float PlayerVelocity = 0.4;               //玩家移动速度（每个内部回合）
const int BurnDamage = 5;                       //每一个灼烧周期玩家损失的HP
const int BurnPeriod = 50;                      //灼烧周期长度（内部回合数）
const int SuckDamage = 70;                      //吸血攻击力
const float SuckDrainCoeff = 0.5;               //吸血攻击回复量等于攻击力乘以该补正系数
const float SuckRange = 5.0;                    //吸血的攻击范围
const int SuckAttackCD = 10;                    //吸血攻击的CD（内部回合数）

const int InitialVillagers = 10;                //初始村民数量
const int WanderIntervalLB = 20;                //村民游走间隔下限（内部回合数）
const int WanderIntervalUB = 50;                //村民游走间隔上限（内部回合数）
const int CorpseLifeTime = 100;                 //尸体的留存时间（内部回合数）

/*
 *  金钱和道具
 */
const int GoldPerSec = 1;                       //每秒玩家获得的金钱
const int GoldPerVillager = 10;                 //击杀村民获得的金钱
const int GoldPerEnemy = 20;                    //击杀敌人获得的金钱
const float DeathPunishment = 1.0;              //死亡时金钱乘以该惩罚系数向下取整

const int BombPrice = 30;                       //炸弹的价格
const int BombCD = 50;                          //使用炸弹的CD（内部回合数）
const float BombTrajectoryTime = 3;             //炸弹的弹道飞行时间（内部回合数）
const int BombDamage = 90;                      //炸弹的伤害量
const float BombRadius = 10.0;                  //炸弹的爆炸半径

const int WardPrice = 5;                        //守卫的价格
const int WardCD = 1200;                        //使用守卫的CD（内部回合数）
const int WardDuration = 600;                   //守卫的持续时间（内部回合数）
const float WardRadius = 15.0;                  //守卫的监视半径

/*
 *  昼夜
 */
const int DayTime = 300;                        //白昼时间（内部回合数）
const float DaySight = 40.0;                    //白昼视野
const int NightTime = 150;                      //夜晚时间（内部回合数）
const float NightSight = 15.0;                  //夜晚视野

enum DayPeriod
{
    Day,
    Night
};

/*
 *  游戏控制
 */
const int RespawnTime = 100;                    //基础重生时间（内部回合数）
const int EnemyKillBonus = 1;                   //击杀敌人获得的基础奖励分数
const int GameSetLimit = 30;                    //使游戏结束的分值

#endif  //LITERALS_H