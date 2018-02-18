//
// Created by IcyF on 2018/2/9.
//

#ifndef SDK_H
#define SDK_H

#include "actions.h"
#include "randomizer.h"
#include "router.h"

namespace SDK
{
    /**
     * reachable: 判断一个点是否可达，仅仅判断点是否位于障碍物之内。
     * @param x 待判断的点的坐标
     * @return 布尔值，可达则为true，否则为false
     */
    bool reachable(Vec2 x);

    /**
     * routeTo: 计算从起点走到终点的一条最短路径。
     * @param start 起点坐标
     * @param end 终点坐标
     * @return 一个点列，描述从start走到end，应当依次沿线段走到哪些点（点列的最后必为终点）
     * @warning 如果输入非法（起点不可达、终点不可达、起点不可抵达终点），那么返回为一个空列表。
     */
    std::vector<Vec2> routeTo(Vec2 start, Vec2 end);

    /**
     * distanceTo: 计算沿最短路径从起点走到终点，要走的距离。
     * @param start 起点坐标
     * @param end 终点坐标
     * @return 起点走到终点的最短距离，通过调用routeTo计算；若routeTo判断不可抵达，该值为FINF
     */
    float distanceTo(Vec2 start, Vec2 end);
}

#endif //SDK_H
