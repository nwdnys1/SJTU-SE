# OLC（基于简单图形引擎的交互式程序设计）

这是本人于大一暑假期间参与的暑期科研见习岗位项目，由汪宇霆老师指导。主要任务是了解并学习使用一种基于 C++的简单图形引擎——OLCPixelGameEngine，实现一些简单的交互式程序（如游戏）。

[olcPixelGameEngine 代码来源](https://github.com/OneLoneCoder/olcPixelGameEngine)

## Battlefield（基于 OLC 实现的小游戏）

Battlefield 是一个简单的回合制战棋类小游戏，游戏在一个矩形的站场上，由 AB 双方交替控制单位来移动与攻击消灭对方单位。本人基于 OLC 引擎实现了一个简单的 Battlefield 小游戏。

由于当初制作的不完善，因此地图的导入以及文件的相对路径都存在问题，因此这个项目只能当做是曾经试试手的练习。未来有机会会进行完善。

- 如何运行？

用 Visual Studio 打开 Battlefiled.sln 项目文件，编译后手动打开位于 x64 文件夹下的 exe 可执行文件即可运行（直接在 VS 里运行会加载不出图片资源，原因未知）。复制 maps 文件夹下的 txt 文件内容并输入到控制台中即可生成地图并开始游戏。

或者直接使用 gcc 工具编译后运行，但是文件路径需要进行修改。

- 如何游玩？

GUI 下方有操作提示。

## Data-Structure-Visualization（基于 OLC 实现的数据结构可视化程序）

一个简单的可视化程序，将链表、二叉树和图结构进行了可视化，类似于[USFCA 实现的可视化网站](https://www.cs.usfca.edu/~galles/visualization/Algorithms.html)

- 如何运行？

用 Visual Studio 打开项目文件，编译运行即可。也可以直接用 gcc 等工具编译运行。

- 如何操作？

可以用鼠标拖拽节点，用相应按键插入、删除或者修改节点。
