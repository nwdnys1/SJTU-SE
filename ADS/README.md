# ADS 高级数据结构

- 包含了 12 份作业，3 个 Lab 以及一个 Project——LSMKV。

## HOMEWORKS

- HW0: Python VS. C++
- HW1: Skiplist
- HW2: Bloom Filter
- HW3: Splay Tree
- HW4: RB-Tree Analysis
- HW5: RB-Tree Implementation
- HW6: KD-Tree
- HW7: Quick Select & Linear Select
- HW8: Dijkstra & KMP
- HW9: Parallel
- HW10: Thread Safety
- HW11: Multi-Thread
- 详见各个作业要求文档

## LABS

### LAB0 哈夫曼压缩

- 实现一个哈夫曼压缩树，对于给定的输入文本文件进行哈夫曼编码并使用新的编码进行压缩，按照一定格式输出压缩后的文本字节流，比较不同压缩方式（单字符编码或多字符编码）下的压缩效果如何。

### LAB1 基数树的压缩

- 实现一个基数树并尝试对其进行压缩，对基数树、压缩基数树与红黑树结构进行了不同工作负载下的 YCSB 性能测试，分析三种结构的性能优势与使用场景。

### LAB2 HNSW

- 实现⼀种基于多层无向图实现的数据结构 HNSW，包括查询、插入操作，并对其查询操作进行简单的多线程的并行优化。HNSW（Hierarchical-Navigable-Small-World）是⼀种基于图的 ANN 索引，主要用于多维特征向量的近似查询，可以应用于商城的推荐系统等向量数据库的查询场景。

## PROJECTS

### PROJECT1 LSMKV

- 实现一种基于 LSM 树结构以及键值分离技术的持久化存储结构，支持 PUT DELETE GET SCAN RESET 等基本操作。进行多种性能测试，包括基本操作的时延、缓存策略的影响、合并操作对于性能的影响以及 Bloom Filter 大小因素的影响。
