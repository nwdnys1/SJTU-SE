#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "config.h"

struct node
{
    key_type key;
    value_t val;
    std::vector<node *> next; // next[i] is the next node of the current node at level i
    node(key_type k, const value_t &v, size_t level) : key(k), val(v), next(level, nullptr) {}
};

class Memtable
{
public:
    node *head;         // 跳表头节点
    double p;           // 跳表层数增加的概率
    int max_level = 32; // 跳表最大层数
    int level = 1;      // 跳表当前层数
    size_t size = 0;    // 转换为SSTable的大小 单位为字节

    int random_level()
    {
        int level = 1;
        while (((double)rand() / RAND_MAX) < p && level < max_level)
            level++;
        return level;
    }

    Memtable(double p = 0.25) : p(p)
    {
        head = new node(-1, "", max_level);
        size = sst_header_size + sst_bf_size;
    }

    // 插入键值对
    void put(key_type key, const value_t &value);

    // 获取键对应的值，如果不存在则返回空字符串
    value_t get(key_type key);

    // 删除键值对
    void del(key_type key);

    // 初始化跳表
    void reset();

    void scan(key_type start, key_type end, std::map<uint64_t, std::string> &scan_result);

    // 释放跳表节点内存
    void freeSkipList(node *n)
    {
        while (n) // 只需要遍历最底层的链表即可删除所有节点
        {
            node *tmp = n;
            n = n->next[0];
            delete tmp;
        }
    }

    void mem2ss();

    ~Memtable()
    {
        freeSkipList(head);
    }
};
