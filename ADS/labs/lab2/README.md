# LAB2&3——HNSW

这是上海交通大学“高级数据结构”课程的 LAB2&3 项目，主要实现了⼀种基于无向图和堆结构实现的数据结构 HNSW，并对于查询操作进行了多线程的并行优化。

HNSW（Hierarchical-Navigable-Small-World）是⼀种基于图的 ANN 索引，主要用于多维特征向量的近似查询，可以应用于商城的推荐系统等向量数据库的查询场景。

## 进行编译

项目可以使用 linux 环境下的 g++进行编译。确保你已经安装了环境所需的工具。

输入

```
$ make all
```

编译正确性测试（hnsw）和并行优化性能测试程序（test_prog），

输入

```
$ make clean
```

以删除所有生成的结果。

## 运行测试

在确保项目已经成功编译的前提下，输入

```
$ ./test_prog
```

运行性能测试程序，程序将输出串行 HNSW 下的插入和查询耗时以及当前线程数下的单次查询平均时延（微秒）。
若需要测试不同的线程数，请打开 test.cpp 对全局变量 thread_num 进行修改。
输入

```
$ make grade
```

运行正确性测试，显示当前参数（位于 util 文件夹的 parameter.hpp 文件中）下 HNSW 的查询召回率（查询得到的局部最优解是全局最优解的概率）以及单次查询平均时延。
若要测试不同参数（比如 M），请打开 parameter.hpp 进行修改。

## 获取实验数据结果

由于实验数据规模较小，采用了手动记录的方式。本人的实验结果记录在了 result.xlsx 文件中，并用 result.py 程序进行了图像的绘制。

## 其余事项

其余事项可以阅读 pdf 文档，包括 HNSW 的伪代码等。

## 贡献者

- 朱涵 SJTU SE2022

## 参考

- [HNSW 原论文](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8594636&tag=1)