#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>

#include "map_reduce/protocol.h"

namespace mapReduce
{

    Worker::Worker(MR_CoordinatorConfig config)
    {
        mr_client = std::make_unique<chfs::RpcClient>(config.ip_address, config.port, true);
        outPutFile = config.resultFile;
        chfs_client = config.client;
        work_thread = std::make_unique<std::thread>(&Worker::doWork, this);
        // Lab4: Your code goes here (Optional).
    }

    void Worker::doMap(std::vector<std::string> &files, std::string inter_file)
    {
        // Lab4: Your code goes here.
        std::vector<KeyVal> inter_data;                          // 中间结果 未进行合并与排序
        std::vector<std::pair<std::string, std::string>> result; // 最终结果

        // Map得到中间结果
        for (auto &file : files)
        {
            std::string file_content;
            Utils::get_file_content(file, file_content, chfs_client);
            auto map = Map(file_content);
            inter_data.insert(inter_data.end(), map.begin(), map.end());
        }
        // 中间结果进行Sort 以便Reduce
        std::sort(inter_data.begin(), inter_data.end(), [](KeyVal const &a, KeyVal const &b)
                  { return a.key < b.key; });

        // Reduce
        auto len = inter_data.size();
        auto i = 0;
        while (i < len)
        {
            std::string key = inter_data[i].key; // 读取key
            std::vector<std::string> values;     // 保存相同key的value
            while (inter_data[i].key == key)     // 将相同key的value保存到values中
            {
                values.push_back(inter_data[i].val); // 读取value
                ++i;
            }
            result.emplace_back(key, Reduce(key, values)); // 将key和对应的value进行reduce
        }

        // 写入文件
        Utils::write_file_content(inter_file, result, chfs_client);
    }

    void Worker::doReduce(std::vector<std::string> &files)
    {
        // Lab4: Your code goes here.
        std::vector<KeyVal> inter_data;
        std::vector<std::pair<std::string, std::string>> result;
        // 读取中间文件
        for (auto &file : files)
        {
            std::string content;
            Utils::get_file_content(file, content, chfs_client);
            auto map = Utils::str2kv(content);
            inter_data.insert(inter_data.end(), map.begin(), map.end());
        }
        // 中间结果进行Sort 以便Reduce
        std::sort(inter_data.begin(), inter_data.end(), [](KeyVal const &a, KeyVal const &b)
                  { return a.key < b.key; });
        // Reduce
        auto len = inter_data.size();
        auto i = 0;
        while (i < len)
        {
            std::string key = inter_data[i].key; // 读取key
            std::vector<std::string> values;     // 保存相同key的value
            while (inter_data[i].key == key)     // 将相同key的value保存到values中
            {
                values.push_back(inter_data[i].val); // 读取value
                ++i;
            }
            result.emplace_back(key, Reduce(key, values)); // 将key和对应的value进行reduce
        }

        // 写入输出文件中
        Utils::write_file_content(outPutFile, result, chfs_client);
    }

    void Worker::doSubmit(mr_tasktype taskType, int index, std::string inter_file)
    {
        // Lab4: Your code goes here.
        mr_client->call(SUBMIT_TASK, (int)(taskType), index, inter_file);
    }

    void Worker::stop()
    {
        shouldStop = true;
        work_thread->join();
    }

    void Worker::doWork()
    {
        while (!shouldStop)
        {
            // Lab4: Your code goes here.
            // 询问任务
            auto task = mr_client->call(ASK_TASK).unwrap()->as<std::tuple<int, int, std::vector<std::string>, std::string>>();
            int idx = (std::get<0>(task));
            mr_tasktype type = (mr_tasktype)(std::get<1>(task));
            std::vector<std::string> files = (std::get<2>(task));
            std::string inter_file = (std::get<3>(task));
            switch (idx)
            {
            case -2: // 任务已完成
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                break;
            }
            case -1: // 没有任务
            {
                break;
            }
            default: // 有任务
            {
                if (type == mr_tasktype::MAP) // map任务
                {
                    this->doMap(files, inter_file);
                    doSubmit(type, idx, inter_file);
                }
                else if (type == mr_tasktype::REDUCE) // reduce任务
                {
                    this->doReduce(files);
                    doSubmit(type, idx, inter_file);
                }
            }
            }
        }
    }
}