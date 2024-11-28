# ICS 计算机系统基础

## Labs

### LAB1 DataLab

考察对于位运算的理解，尝试只使用给定的运算实现不同的函数。

### LAB2 BombLab

拆炸弹模拟器，考察对于汇编语言以及 gdb 调试工具的掌握。

### LAB3 AttackLab （找不到自己的答案了）

黑客模拟器，考察对于汇编语言的理解以及对于程序运行栈的理解，尝试对现有程序进行攻击。

- 用于参考：https://blog.csdn.net/weixin_53016579/article/details/130327473

### LAB4 Tiny Shell

一个简单的 Unix Shell，考察对于并发、进程、信号的掌握。

- 用于参考：https://zhuanlan.zhihu.com/p/492645370

### LAB5 Y86-64 Simulator

实现一个简单的 Y86 模拟器，读取 Y86 汇编代码并模拟运行。

### LAB6 Y86-64 Assembler

实现一个简单的 Y86 汇编器，读取 Y86 汇编语音并转换为二进制的机器语言。

### LAB7 Architecture Lab

实现一个简单的 PIPELINE 模拟处理器，首先写一些简单的 Y86 程序，然后根据一个新指令扩展 SEQ 处理器的代码，最后利用新的指令以及你对于流水线处理器的理解进行优化，尝试得到尽可能高的性能。

- 用于参考：https://zhuanlan.zhihu.com/p/480380496

### LAB8 Cache Lab

实现一个简单的 CACHE 模拟器，并尝试根据缓存的原理优化一个计算矩阵乘法的程序，尝试得到尽可能高的性能。

- 用于参考：https://zhuanlan.zhihu.com/p/484657229

### LAB9 Malloc Lab

实现一个动态分配器，完成 malloc，free，realloc 等功能，并根据对于内存组织形式的理解优化你的程序，尝试得到尽可能高的性能。（最终参考大佬的分离链表实现，95 分）

- 用于参考：https://zhuanlan.zhihu.com/p/484657229

### LAB10 Proxy Lab

实现一个作为代理的多线程 web 程序，解析客户端的请求并发送给服务端，解析服务端的响应并发送回客户端。

- 用于参考：https://zhuanlan.zhihu.com/p/497982541（和ICS课程给的文档要求有所不同）
- 我的代码实际上大部分参考了软院往届学长的开源代码，因此如果需要参考可以直接去 awesome SE 项目进行查看。
