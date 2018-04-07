#ifndef LITERALS_H
#define LITERALS_H

const float PI = 3.14159265f;
const float EPS = 1e-4;
const int IINF = 99999999;
const float FINF = 99999999.0f;
const unsigned int MAGIC = 0x30a54bf8;          //Magic Number!

/*
 *  地图信息
 */
const float MapWidth = 60.0;
const float MapHeight = 90.0;
const float ObsBorder = 0.05;                   //障碍物边界向外延伸的不可达区域宽度

/*
 *  玩家和村民
 */
const int PlayerHP = 100;                       //玩家出生时HP
const float PlayerVelocity = 1.2;               //玩家移动速度（每个内部回合）
const int BurnDamage = 6;                       //每一个灼烧周期玩家损失的HP
const int BurnPeriod = 10;                      //灼烧周期长度（内部回合数）
const int SuckDamage = 50;                      //吸血攻击力
const float SuckDrainCoeff = 0.5;               //吸血攻击回复量等于攻击力乘以该补正系数
const float SuckRange = 3.0;                    //吸血的攻击范围
const int SuckAttackCD = 6;                     //吸血攻击的CD（内部回合数）

const int InitialVillagers = 5;                 //初始村民数量 * 玩家人数
const int WanderIntervalLB = 15;                //村民游走间隔下限（内部回合数）
const int WanderIntervalUB = 30;                //村民游走间隔上限（内部回合数）
const int CorpseLifeTime = 50;                  //尸体的留存时间（内部回合数）

/*
 *  金钱和道具
 */
const int Salary = 1;                           //玩家获得的工资金钱
const int SalaryPeriod = 5;                     //发放工资的周期
const int GoldPerVillager = 6;                  //击杀村民获得的金钱
const int GoldPerEnemy = 15;                    //击杀敌人获得的金钱
const float DeathPunishment = 0.8;              //死亡时金钱乘以该惩罚系数向下取整

const int BombPrice = 20;                       //炸弹的价格
const int BombCD = 30;                          //使用炸弹的CD（内部回合数）
const int BombTrajectoryTime = 3;               //炸弹的弹道飞行时间（内部回合数）
const int BombDamage = 90;                      //炸弹的伤害量
const float BombRadius = 10.0;                  //炸弹的爆炸半径

const int WardPrice = 5;                        //守卫的价格
const int WardCD = 150;                         //使用守卫的CD（内部回合数）
const int WardDuration = 50;                    //守卫的持续时间（内部回合数）
const float WardRadius = 15.0;                  //守卫的监视半径
const float WardPlaceRadius = 3.0;              //守卫的放置半径

/*
 *  昼夜
 */
const int DayTime = 80;                         //白昼时间（内部回合数）
const float DaySight = 110.0;                   //白昼视野
const int NightTime = 60;                       //夜晚时间（内部回合数）
const float NightSight = 10.0;                  //夜晚视野

enum DayPeriod
{
    Day,
    Night
};

/*
 *  游戏控制
 */
const int RespawnTime = 30;                     //基础重生时间（内部回合数）
const int EnemyKillBonus = 1;                   //击杀敌人获得的基础奖励分数
const int GameSetLimit = 20;                    //使游戏结束的分值

#endif  //LITERALS_H