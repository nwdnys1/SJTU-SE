#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include "MurmurHash3.h"
#include "config.h"

class BloomFilter
{
public:
    int m_ = sst_bf_size * 8;                   // Bloom filter size (bits) 8192 * 8 bits = 8KB
    int k_ = 4;                                 // Number of hash functions
    std::bitset<sst_bf_size * 8> bloom_filter_; // Bloom filter bitset

    BloomFilter()
    {
        bloom_filter_.reset(); // 初始化为0
        
    }

    ~BloomFilter()
    {
        
    }

    // 插入元素
    void insert(key_type element)
    {
        unsigned int hash[k_] = {0};                             // 128位hash值 分为4个无符号整数
        MurmurHash3_x64_128(&element, sizeof(element), 1, hash); // 计算hash值
        bloom_filter_.set(0, true);
        for (int j = 0; j < k_; j++)
            bloom_filter_.set(hash[j] % m_, true); // 将hash值对应的位置设为1
    }

    bool check(key_type element)
    {
        unsigned int hash[k_] = {0};                             // 128位hash值 分为4个无符号整数
        MurmurHash3_x64_128(&element, sizeof(element), 1, hash); // 计算hash值
        for (int j = 0; j < k_; j++)
            if (bloom_filter_.test(hash[j] % m_) == false) // 如果有一个位置为0 则返回false
                return false;
        return true;
    }
};
