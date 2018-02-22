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

| 变量名称 | 类型 | 含义  |
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

`emplace` 函数的原型如下：

```cpp
void emplace(ActionType type, int target_id, Vec2 pos)
void emplace(ActionType type, int target_id);
void emplace(ActionType type, Vec2 pos);
```

`ActionType` 是一个枚举类型，代表了动作的类型，可取值如下：

| 取值 | 含义 | 必须指定的参数 |
|--|--|--|
| NoAction | **仅作为内部取值而使用**：无意义的空动作 | - |
| SelectDestination | 选择一个目标地点，并按照寻路系统给出的最短路开始向该点移动 | pos |
| ContinueMovement | **仅作为内部取值而使用**：指示逻辑按照上一个 SelectDestination 决定的路线继续移动 | - |
| BuyItem | 购买道具 | target_id |
| UseItem | 使用道具 | target_id, pos |
| SuckAttack | 普通攻击 | target_id |

NoAction 和 ContinueMovement 仅作为逻辑内部的取值而被使用，如果您向逻辑报告了 NoAction 或 ContinueMovement 动作，它们将被直接忽略。

## 使用 SDK

我们提供一个轻量的 SDK 供您使用，使用 SDK 可以在一定程度上简化您的编程；对于刚接触本游戏的初学者而言，您还可以借助 SDK 加深对游戏机制和核心玩法的了解。

要使用 SDK 只需包含 `sdk/sdk.h`。SDK 的全部内容包含在命名空间 `SDK` 中。

### SDK 函数

我们一共只提供 3 个功能性的函数：

```cpp
bool reachable(Vec2 x);
std::vector<Vec2> routeTo(Vec2 start, Vec2 end);
float distanceTo(Vec2 start, Vec2 end);
```

这三个函数是对 `sdk/router.h` 中 `Router` 类接口的再封装。 `Router` 类应用了单例模式，与直接调用 `Router` 类的接口相比，使用 SDK 封装好的函数能降低您代码的复杂程度。

三个函数的意义分别如下：

| 函数名 | 含义 |
|--|--|
| reachable | 判定 x 点是或不是一个合法位置（关于合法位置的定义，请参阅：逻辑参考文档） |
| routeTo | 给出从 start 到 end 的一条最短路径。<br />返回值是一个点列，表示从 start 移动到 end，应当先由 start 处移动到点列的第 1 点处，然后由点列的第 1 点处移动到点列的第 2 点处……，最后移动到点列的最后一点处，该点必定为 end。上述每次移动均为直线移动，中间没有障碍。<br />如果 start 或 end 是非法位置，那么返回值为一个空 vector。 |
| distanceTo | 根据 routeTo 给出的最短路线，计算从 start 到 end 的路程。<br />如果 start 或 end 非法，那么返回 FINF（定义在 `sdk/literals.h`）。 |

### 预定义策略

`SDK` 命名空间中包含的第四个函数允许您使用预定义的策略，方便您了解游戏的核心玩法。该函数的原型如下：

```cpp
ActionMaker* actionMaker();
```

actionMaker 函数保存一个静态的 ActionMaker 局部变量，并且返回指向它的指针；通过这种方式，我们为您封装好了一个 ActionMaker 类的单例。

您可以利用该单例调用预定义策略。预定义策略是我们为您提供的一些简单的游戏策略，您可以选取其中一些并应用它们；对于初学者而言，您可以尝试各种不同的组合；对于已经熟悉游戏的选手，您可以利用这些策略，某种程度上减低您的代码量。

所有的预定义策略都分别以类的形式实现，这些类全部公有继承于基类 Strategy（定义于 `sdk/strategy/strategy.h`）。ActionMaker 提供了一个接口供您向其中添加策略，它的原型如下：

```cpp
template <typename Tp, typename... Types>
void addStrategy(std::string name, Types... args);
```

其中 name 是策略的名称，该名称由您来指定。您指定的各个名称应当互不重复，相同名称的策略会发生覆盖。如果您到时希望移除某个策略，您需要指定被移除策略的名称。

addStrategy 是一个可变参数模板函数，它的可变参数部分是为了构造预定义策略类，因为某些预定义策略需要给定构造参数才能正常工作。该函数的使用方式就像下面这样：

```cpp
SDK::actionMaker()->addStrategy<StealthStrategy>("sample_name", 30);
```

也就是说，通过模板参数的方式指定您选择的策略，通过函数形参指定名称，以及策略所需的构造函数参数。

上述代码中，类 StealthStrategy 是我们提供的预定义策略之一，其定义位于 `sdk/strategy/stealth_strategy.h`（该目录下每个以 `_strategy.h` 结尾的头文件中都包含了一个预定义策略）。`30` 则是 StealthStrategy 的构造函数参数。关于各个预定义策略的具体内容和构造参数，请参阅下表：

| 预定义策略类 | 构造参数 | 策略内容 |
|--|--|--|
| EscapeStrategy | tries（整型，可选） | 每回合随机选取 tries 个方向，并且向离视野内单位最远的那个方向移动 |
| FocusAttackStrategy | id（整型，必须） | 使用普通攻击和炸弹尽可能攻击编号为 id 的单位 |
| PatrolStrategy | 无 | 持续按逆时针方向环绕正方形场地，每次在抵达正方形的一个角之后随机暂停一段时间 |
| RandomizedAttackerStrategy | 无 | 持续对身边的随机单位使用普通攻击，每个编号最多攻击一次 |
| StealthStrategy | hp_lower_bound（整型，默认为 20） | 在 HP 高于 hp_lower_bound 时模拟村民运动；否则，如果有单位在普通攻击范围内，则攻击离自身最远的，并向他当回合速度的反方向逃离；如果没有，则朝视野内离自己最近的单位运动 |
| TracerStrategy | id（整型，必须）、distance（浮点型，默认为 5.0） | 跟踪编号为 id 的单位，保持距离不小于 distance<br />如果该单位从视野中消失，立刻原地停止 |

为了应用您添加好的策略，您只需在 `playerAI` 函数中调用：

```cpp
SDK::actionMaker()->make(sight, actions);
```

ActionMaker 就会自动应用这些策略并向 actions 中添加对应的动作。

* 需要注意的是，策略在添加后始终有效，直到它被移除。您不需要每回合都重复添加同样的策略。

您也可以自行继承 Strategy 类并实现您自己的策略：首先您需要包含 `sdk/strategy/strategy.h` 并继承纯虚类 Strategy，在继承类中实现策略函数：

```cpp
virtual void generateActions(const PlayerSight &sight, Actions *actions);
```

此外，您还可以重写 getStrategyType 函数为您的策略指定一种类型（Defensive、Neutral 或 Offensive）。ActionMaker 支持按类型批量地移除策略。

## 总结

通过以上的说明，您应该已经对 AI 的编写方式和 SDK 的使用有了一个大致的了解。

感谢您的阅读，祝您参赛愉快。