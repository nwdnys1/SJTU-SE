# SEP（软件基础实践）Labs&Projects

## Labs

### LAB1 X 老师的小程序

实现一个统计和计算学生成绩的命令行程序。

### LAB2 MiniEd

实现一个简化版的 Ed 编辑器（一种行文本编辑器）。

### LAB3 汉诺塔

实现一个可视化的汉诺塔游戏命令行界面程序。

### LAB4 2D-Tree

实现一个二维树的数据结构，并实现 KD 树里的最近邻算法。

### LAB5 Boggle Game（由于我们这届 SEP 少一个 lab 因此文档里显示 lab6）

实现一个命令行的 Boggle 游戏，利用 dfs 回溯算法搜索所有可行解。

### Debug Lab 1 （本届新增）

一个简单的 lab，对五个存在 bug 的程序进行调试。

### Debug Lab 2 （本届新增）

在这个 lab 里，你需要对助教写的一个 B 树结构进行调试，找到所有问题所在并汇报你是如何找到并解决 bug 的。

- 注：本人能力有限，未能做出这个 lab，而且获得了 20 分的最低分，学弟学妹们可以参考其他前辈的解答 TT

## Projects

### QLink

使用 Qt 实现一个简单的基于 RPG 形式的连连看游戏 主要考察对于 Qt 中事件处理以及渲染的理解，工作量较大，但是难度并不高，适合入门 Qt 学习用，不过主要靠自学，多看文档多搜百度即可解决大部分问题。

- 如何编译运行我的项目？

  点击 llk.pro 文件打开项目，点击 Configure Project 进行配置，然后运行即可。

- 我的连连看游戏的简单介绍：

  UI 比较简陋，本人应该也不会再花时间进行美化了。基本功能都是完善的，最终答辩助教的测试点都没有问题，获得 98 的分数。

  选择模式开始游戏，方向键操控角色触碰方块进行消除。道具说明可以点击左上角的帮助进行查看。支持保存和加载存档。倒计时结束或没有可以消除的方块时结束游戏并结算分数。

  测试功能需要将 main 函数注释掉再进行测试（因为不能有两个主函数入口）。

- 小 TRICK：代码风格要求可以通过全部复制到 VSCode 或者 VS 里一键 format 一下然后再复制回去，虽然我也不知道代码风格是否真的算了分...

### QBasic

QBasic 语言是 1975 年微软开发的一种解释型语言，专门用于教学和初学者学习编程。在这个项目中，你需要实现一个 QBasic 语言解释器的 GUI 界面，可以存储、编译以及运行 QBasic 语言编写的代码。本项目可能的难点有：对于语法树的理解，对于语言的编译与运行时处理。工作量较小，完成这个项目对于理解代码的编译和运行原理有不小的帮助。

- 如何编译运行我的项目？

  同 QLink。

- 我的 QBasic 解释器的简单介绍：

  UI 基于原代码的 UI。基本功能完善，包括编译、存储、运行、展示语法树等。最终版本有一个小 bug，导致答辩时扣了一点分，最终获得 93 分，不过距今已有些久远，所以这个 bug 我也不记得具体是什么了，若用于参考还请注意（笑）。

## Exam

每一届都有的上机考试，考察内容是现场根据要求实现一个 Qt 的小程序。

本人得出的经验：要考察 Qt，又要结合本届学过的算法知识，只可能是结合 dfs 回溯算法了（因为其他根本就什么都没学）。上一届的机考是实现一个寻找穿越战场的合法路径的可视化程序，本届则是帮助小雪人安全穿越火堆。因此每一届大概率是考一个可视化的迷宫寻找合法路径的问题。基本思路也很简单，用 bfs 或者 dfs 回溯即可。也许会有一些不一样的附加要求，但是都不难，合法路径的寻找就是核心。

- 如何编译运行我的项目？

  同上。

- TIPS：
  - 可以先把往届的题目实现了练练手，然后因为机考是断网开卷的，所以你完全可以像我一样直接把核心的路径寻找算法 copy 上去，这会节省很多时间，要不然如果不熟练的话 3 个小时从头开始手写是不一定来得及的，毕竟不联网也用不了 copilot 补全（笑）。
  - 注意地图的二维数组，本人在测试时才发现把 x 轴和 y 轴搞反了，因此导致所有非正方形的地图测试都错了。好在助教比较宽容，没有因此扣分，最终是满分。