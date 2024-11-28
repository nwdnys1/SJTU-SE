#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include "bloom_filter.h"

struct Header
{
    uint64_t min_key;   // 最小key
    uint64_t max_key;   // 最大key
    uint64_t tuple_num; // tuple数量
    uint64_t time;      // 时间戳
};

struct Tuple
{
    key_type key;    // key
    uint64_t offset; // 偏移量
    uint32_t vlen;   // value长度
};

class SSTable
{
public:                   // 为方便 一律设为public
    std::string filepath; // 文件路径
    uint64_t filesize;    // 文件大小 单位为字节

    // 一个SSTable文件由三部分组成：header、BloomFilter、tuple
    Header header;             // header
    BloomFilter bloomfilter;   // bloomfilter
    std::vector<Tuple> tuples; // tuple

    SSTable(std::string path) : filepath(path)
    // 这个构造函数有两个功能：1.如果文件不存在 则创建一个空文件 2.如果文件存在 则读取文件内容放到缓存中
    {
        std::ifstream in(filepath, std::ios::binary | std::ios::in); // 以二进制读方式打开文件
        if (!in)                                                     // 如果文件不存在
        {
            filesize = sst_bf_size + sst_header_size;
            header.min_key = 0;
            header.max_key = 0;
            header.tuple_num = 0;
            header.time = 0;
            std::fstream create(filepath, std::ios::out | std::ios::binary); // 创建文件
            create.close();
            return;
        }
        // 读取SSTable文件头部信息 注意顺序
        in.read((char *)&header.time, sizeof(header.time));
        in.read((char *)&header.tuple_num, sizeof(header.tuple_num));
        in.read((char *)&header.min_key, sizeof(header.min_key));
        in.read((char *)&header.max_key, sizeof(header.max_key));
        // 读取bloomfilter
        in.read((char *)&(bloomfilter.bloom_filter_), sizeof(bloomfilter.bloom_filter_));
        int n = header.tuple_num;
        for (int i = 0; i < n; i++) // 读取tuple
        {
            Tuple tmp;
            in.read((char *)&tmp.key, sizeof(tmp.key));
            in.read((char *)&tmp.offset, sizeof(tmp.offset));
            in.read((char *)&tmp.vlen, sizeof(tmp.vlen));
            tuples.push_back(tmp);
        }
        in.close();
    };
    SSTable(){
        // 无参构造函数 用于compaction时创建一个空的SSTable 但是不创建任何文件
    };
    ~SSTable();
};
