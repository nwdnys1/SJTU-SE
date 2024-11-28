#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include "config.h"

struct vLog_entry
{
    uint8_t magic;     // 开始标志
    uint16_t checksum; // 校验和
    key_type key;
    uint32_t vlen;
    value_t value;
};

class VLog
{
public:
    std::string filepath; // 文件路径
    size_t filesize;      // 文件大小 单位为字节 也就是head
    size_t tail;          // 用于记录当前vlog文件的尾部位置 单位为字节
    uint8_t magic = 0xff; // 开始标志
    // std::vector<vLog_entry *> entries; // 一个vlog文件由多个entry组成

    VLog(std::string path) : filepath(path)
    // 这个构造函数有两个功能：1.如果文件不存在 则创建一个空文件 2.如果文件存在 则读取文件内容 获得文件大小和尾部位置等信息 但是不缓存entry
    {
        std::ifstream in(filepath, std::ios::binary | std::ios::in);
        if (!in) // 如果文件不存在
        {
            filesize = 0;
            tail = 0;
            std::fstream create(filepath, std::ios::out | std::ios::binary); // 创建文件
            create.close();
            return;
        }
        // 文件大小
        in.seekg(0, std::ios::end);
        filesize = in.tellg();
        // 寻找文件尾部
        in.seekg(utils::seek_data_block(filepath));
        char c;
        uint16_t checksum;            // 校验和
        std::vector<unsigned char> v; // key vlen value组成的二进制序列
        while (in.get(c))
        {

            if (c != (char)magic) // 找到magic
                continue;
            int pos = in.tellg() - 1;                                       // 记录c的位置 因为get会使指针后移 所以要减1
            in.read(reinterpret_cast<char *>(&checksum), sizeof(uint16_t)); // 读取校验和
            v.clear();
            // 读取key vlen value组成的二进制序列 直到遇到magic
            while (in.get(c))
            {
                if (c == (char)magic)
                {
                    in.seekg(-1, std::ios::cur); // 回退一个字节 下一个循环会读取到magic
                    break;
                }
                v.push_back(c);
            }
            if (utils::crc16(v) == checksum) // 校验和正确
            {
                tail = pos; // 更新尾部位置
                return;
            }
        }
        tail = filesize; // 找不到尾部 说明文件全是空洞 则文件尾部是文件大小
        std::cout << "vlog file is empty" << std::endl;
        in.close();
    };
    ~VLog();
};
