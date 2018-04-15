# AI 编写指导教程

下面是我们推荐的 AI 编写方法。

如果您有充足的信心与能力，您完全可以自行设计编译和测试流程。

## 编写 AI

下载的 SDK 中包含一个 playerAI.cpp，我们推荐您修改此文件并在其中完整实现您的 AI。

我们不推荐您改变下载得到的 SDK 的目录结构。

## 编译 AI

您需要使用 CMake 来编译您的 AI。

### 获取 CMake

请访问 <a href="https://cmake.org/download/">CMake 官网</a> 下载 CMake 并安装。

### 使用 CMake 生成 Makefile

在命令行中运行如下命令来打开 CMake GUI：

``` shell
cmake-gui
```

CMake GUI 的界面如下所示：

<div align=center> <img src="c02.png" width="500" /> </div>

* 在 **Where is the source code** 处选择存放 playerAI.cpp 的目录；
* 在 **Where to build the binaries** 处选择存放编译结果的目录，不推荐与上面相同；
* 单击 **Configure** 按钮，在 **Specify generator for this project** 处选择 **MinGW Makefiles**（Windows）或 **Unix Makefiles**（Linux/Mac OS）并单击 **Finish** 确认；
* 单击 **Generate** 按钮。

在您看到 Generating done 之后，就可以退出 CMake GUI：

<div align=center> <img src="c03.png" width="500" /> </div>

如果您使用的是 Windows 系统并且找不到 MinGW Makefiles 一项，请安装 TDM-GCC。可以在 Arena9 官网的贺岁赛文件下载处找到安装程序。

### 编译

进入您在 **Where to build the binaries** 处选择的目录并打开命令行，运行

``` shell
make
```

如果编译成功，命令行应该显示 Built target playerAI：

<div align=center> <img src="c04.png" width="700" /> </div>

使用 Linux/Mac OS 的选手不会见到 warning: -fPIC ignored for target 的提示信息。

编译成功后，您应当能在这个目录下发现名为 libplayerAI.dll/.so/.dylib 的动态链接库文件。这即是编译得到的最终结果。

当您修改 AI 代码而需要重新编译时，只需重新执行 make 指令，无须再重新生成 Makefile。

## 评测您的 AI

我们提供的本地评测程序包含两个文件：一个可执行文件 judger，以及一个动态链接库 liblib_logic。

动态链接库 liblib_logic 封装了整个游戏逻辑，judger 通过加载它来完成选手 AI 的评测。

judger 接受命令行参数，用以指定一局比赛的游戏人数、每名玩家的 AI 路径、最大回合数以及回放文件存放位置。您可以不加任何参数地调用 judger 来获取有关命令行参数的说明：

``` shell
./judger
```

例如，假如您已经编译得到了 libplayerAI.dll，并且想要用这个 AI 来进行一场本地的 2 人比赛，您可以先将 libplayerAI.dll 复制到存放 judger 的目录下，运行

``` shell
./judger -players=2 -path0="libplayerAI.dll" -path1="libplayerAI.dll"
```

judger 在成功评测一局比赛后产生一行输出，为按名次从高到低排列的各选手编号。judger 同时还在您指定的位置产生一个回放文件，该文件为 zip 格式，压缩包中包含比赛回放文件 replay.json，您需要解压出 replay.json 供播放器读取。

## FAQ

### CMake

**Q: ** CMake 提示 **Error in configuration progress, project files may be invalid** 如何处理？

**A: ** 请确保您存放 SDK 的路径不包含中文字符。

<br />
**Q: ** CMake 提示 **The source ... does not match the source ... used to generate cache** 如何处理？

**A: ** 请删除 CMake 产生的 CMakeFiles 文件夹和 CMakeCache.txt，然后重新 Configure。

<br />
**Q: ** Configure 和 Generate 都成功了，但 make 失败如何处理？

**A: ** 请先检查您 C++ 编译器的版本是否足够高，如果您的编译器不支持 C++ 11，请更新您的编译器。如果您确认您的编译器版本没有问题，那么您可能使用的是 MinGW，您需要将 make 命令替换为 mingw32-make；或者您也可以打开 MinGW 安装目录下的 bin 目录，将 mingw32-make.exe 重命名为 make.exe。

## 注意

我们在测试时曾经发现一个问题：在 AI 函数中使用指向类实例的静态指针类型，可能导致结果不正确。

具体原因并未明确，如果可能，请您回避使用静态指针类型；如果您发现了相同的问题，请反馈给我们。

## 其他

如果您熟悉 CMake 的使用方法，您可以尝试修改 CMakeLists.txt 以达到您想要的效果（例如，建立一个多文件工程）。

我们不提供这方面的技术指导。
