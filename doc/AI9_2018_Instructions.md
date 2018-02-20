# 2018年智能体大赛 AI编写指导教程

**阅读提示：本文档仅供已经了解游戏规则，并希望编写AI的选手参考。如果您只是希望了解本游戏，请关闭本文档，出门右转，以免对本游戏丧失兴趣。**

## 前言

欢迎您参加 2018 年智能体大赛。如果您阅读到此处，想必您已经熟悉逻辑运作的模式，并且做好编写 AI 并参赛的准备。为了您的方便，我们向您提供这份指导教程，使您能了解逻辑与 AI 的交互方式，以及 SDK 的使用方法。

如您认为本文档中有明显的错误，欢迎您在智能体大赛 QQ 群中提出，或者联系<a href="mailto:icicle.flw@gmail.com">icicle.flw@gmail.com</a>。我们将尽快给出回复。

## 基础类型

为了存储数据的方便，我们定义了若干结构体类型；在说明逻辑如何与 AI 交互之前，我们先介绍这些结构体类型，以使您能顺畅理解接下来的内容。

在这一节我们并不作过于详细的介绍。如果您希望了解更多，请参阅源代码。

### Vec2

`Vec2` 类型定义于 `sdk/vec2.h`。它包含两个 `float` 类型的参数 `x` 和 `y`。如它的名字那样，该类型用于存储二维向量，例如玩家的速度和位置等信息。

### PWardInfo & PBombInfo

`PWardInfo` 和 `PBombInfo` 类型均定义于 `sdk/defs.h`。这两个类型分别用于存储**您看到的**守卫和炸弹信息。

`PWardInfo` 包含两个成员变量：`pos` 和 `life`。`pos` 是 `Vec2` 类型，表示守卫的位置；`life` 是整型，表示它剩余的回合数。

`PBombInfo` 包含两个成员变量：`pos` 和 `velocity`，均是 `Vec2` 类型，分别表示炸弹的位置和飞行速度。

### PUnitInfo

`PUnitInfo` 类型定义于 `sdk/defs.h`，用于存储**您看到的**单位信息。

`PUnitInfo` 包含三个成员变量：`pos`、`velocity` 和 `id`。`pos` 和 `velocity` 是 `Vec2` 类型，表示单位的位置和速度；`id` 是整型，表示**您看到的**单位的 ID。

### Action

`Action` 类型定义于 `sdk/defs.h`，用于存储 AI 每回合给出的动作。

## 逻辑与 AI 的交互

逻辑通过调用 UI 指定的 DLL 与 AI 进行交互。您必须编写指定的函数供逻辑调用。该函数的原型如下：

```cpp
__declspec(dllexport) void playerAI(const PlayerSight sight, Actions* actions);
```

为了使用 `PlayerSight` 和 `Actions` 结构体，您应该在代码中包含 `sdk/sdk.h`。

### 使用 PlayerSight 结构体获取视野

PlayerSight 的定义位于 `sdk/defs.h`。它不包含成员函数，仅仅是封装了一些成员变量。各成员变量及它们的含义如下：

| 变量名称 | 类型 | 意义  |
|--|--|--|
| round | `int` | 当前的回合数，从 1 开始递增 |
| id | `int` | 玩家被分配到的 ID |
| pos | `Vec2` | 玩家的位置 |
| velocity | `Vec2` | 玩家的速度 |
| hp | `int` | 玩家的 HP |
| gold | `int` | 持有的金钱 |
| score | `int` | 分数 |
| continuousKill | `int` | 连杀数 |
| bombCount | `int` | 持有炸弹的数目 |
| wardCount | `int` | 持有守卫的数目 |
| canUseBomb | `bool` | 指示玩家是否能使用炸弹（CD清零） |
| canUseWard | `bool` | 指示玩家是否能使用守卫（CD清零） |
| canSuckAttack | `bool` | 指示玩家是否能普通攻击（CD清零） |
| placedWardCount | `int` | 属于玩家的、正在生效的守卫数目 |
| placedWard | `std::vector<PWardInfo>` | 属于玩家的、正在生效的守卫信息 | 
| bombInSightCount | `int` | 玩家视野中出现的炸弹数目 |
| bombInSight | `std::vector<PBombInfo>` | 玩家视野中出现的炸弹信息 |
| unitInSightCount | `int` | 玩家视野中的**其他**单位数目 |
| unitInSight | `std::vector<PUnitInfo>` | 玩家视野中的**其他**单位信息 |
| corpseInSightCount | `int` | 玩家视野中的尸体数目 |
| corpseInSight | `std::vector<Vec2>` | 玩家视野中的尸体位置 |
| scoreBoard | `std::vector<int>` | 全局计分板，从高到低排列 |

以上也就是您每回合能得知的全部信息。

### 使用 Actions 结构体向逻辑报告本回合动作

Actions 的定义位于 `sdk/actions.h`。它仅仅封装了一个 `std::vector<Action>` 变量，同时提供 `emplace` 函数用以方便您向其中添加动作。

