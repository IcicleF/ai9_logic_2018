#include "router.h"
#include "randomizer.h"
#include <algorithm>

using namespace std;

#define MAX_POINTS 100

Router* Router::getInstance()
{
    static Router instance;
    return &instance;
}
Router::Router()
{
    ObstacleCount = 4;
    Obstacles.resize(4);

    setObstacle(Obstacles[0], 5.0, 5.0, 15.0, 15.0);
    setObstacle(Obstacles[1], -15.0, 5.0, -5.0, 15.0);
    setObstacle(Obstacles[2], -15.0, -15.0, -5.0, -5.0);
    setObstacle(Obstacles[3], 5.0, -15.0, 15.0, -5.0);
}
bool Router::Reachable(Vec2 pos)
{
    for (int i = 0; i < ObstacleCount; ++i)
        if (pos.x >= Obstacles[i][0].x - ObsBorder
            && pos.x <= Obstacles[i][2].x + ObsBorder
            && pos.y >= Obstacles[i][0].y - ObsBorder
            && pos.y <= Obstacles[i][2].y + ObsBorder)
            return false;
    return true;
}
Vec2 Router::availablePosition()
{
    Vec2 res = Randomizer::getInstance()->randPosition();
    while (!Reachable(res))
        res = Randomizer::getInstance()->randPosition();
    return res;
}
std::vector<Vec2> Router::Route(Vec2 start, Vec2 end)
{
    std::vector<Vec2> res;
    if (!Reachable(start) || !Reachable(end))
        return res;
    if ((end - start).length() < EPS)
    {
        res.push_back(end);
        return res;
    }

    int Points = ObstacleCount * 4 + 2;

    float dis[MAX_POINTS];					//起点到该点的最短距离
	float edges[MAX_POINTS][MAX_POINTS];    //边权
	Vec2 vertex[MAX_POINTS];			        //参与最短路计算的点的坐标
	int prev[MAX_POINTS];					//点的前驱，回溯最短路用
	bool used[MAX_POINTS];                  //点是否已松弛过

    //todo: 已知的连线做硬编码，或者预存
    vertex[0] = start, vertex[Points - 1] = end;
    for (int i = 0; i < ObstacleCount; ++i)
    {
        int j = 1 + i * 4;
        vertex[j + 0] = Obstacles[i][0] + Vec2(-ObsBorder, -ObsBorder);
        vertex[j + 1] = Obstacles[i][1] + Vec2(-ObsBorder, ObsBorder);
        vertex[j + 2] = Obstacles[i][2] + Vec2(ObsBorder, ObsBorder);
        vertex[j + 3] = Obstacles[i][3] + Vec2(ObsBorder, -ObsBorder);
    }
    for (int i = 0; i < Points; ++i)
    {
        prev[i] = -1;
        used[i] = false;
        dis[i] = (i == 0 ? 0 : FINF);
        edges[i][i] = 0;
        for (int j = i + 1; j < Points; ++j)
        {
            bool flag = true;
            for (int u = 0; u < ObstacleCount; ++u)
            {
                for (int v = 0; v < 4; ++v)
                    if (intersect(vertex[i], vertex[j], Obstacles[u][v], Obstacles[u][(v + 1) & 3]))
                    {
						flag = false;
                        break;
                    }
                if (!flag)
                    break;
            }
            edges[i][j] = edges[j][i] = (flag ? (vertex[i] - vertex[j]).length() : FINF);
        }
    }

    //Dijkstra
    while (true)
    {
        int v = -1;
        for (int i = 0; i < Points; ++i)
            if (!used[i] && (v == -1 || dis[i] < dis[v]))
                v = i;
		if (v == -1)
            break;
        used[v] = true;
        for (int i = 0; i < Points; ++i)
            if (dis[i] > dis[v] + edges[v][i])
            {
                dis[i] = dis[v] + edges[v][i];
                prev[i] = v;
            }
    }

    int i;
	for (i = Points - 1; i != 0; i = prev[i])
    {
        if (i == -1)
        {
            res.clear();
            return res;
        }
        res.push_back(vertex[i]);
    }
    reverse(res.begin(), res.end());
    return res;
}