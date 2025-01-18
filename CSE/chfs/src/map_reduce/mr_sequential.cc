#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include "map_reduce/protocol.h"

namespace mapReduce
{
    SequentialMapReduce::SequentialMapReduce(std::shared_ptr<chfs::ChfsClient> client, const std::vector<std::string> &files_, std::string resultFile)
    {
        chfs_client = std::move(client);
        files = files_;
        outPutFile = resultFile;
        // Your code goes here (optional)
    }

    void SequentialMapReduce::doWork()
    {
        // Your code goes here
        std::vector<KeyVal> inter_data;                          // 中间结果 未进行合并与排序
        std::vector<std::pair<std::string, std::string>> result; // 最终结果

        // Map得到中间结果
        for (auto &file : this->files)
        {
            std::string file_content;
            Utils::get_file_content(file, file_content, chfs_client);
            auto map = Map(file_content);
            inter_data.insert(inter_data.end(), map.begin(), map.end());
        }

        // 中间结果进行Sort 以便Reduce
        // for (int i = 0; i < 100; i++)
        std::sort(inter_data.begin(), inter_data.end(), [](KeyVal const &a, KeyVal const &b)
                  { std::string a_key = a.key, b_key = b.key; return a_key < b_key; });

        // Reduce
        auto len = inter_data.size();
        auto i = 0;
        while (i < len)
        {
            std::string key = inter_data[i].key;
            std::vector<std::string> values; // 保存相同key的value
            while (inter_data[i].key == key) // 将相同key的value保存到values中
            {
                values.push_back(inter_data[i].val);
                ++i;
            }
            result.emplace_back(key, Reduce(key, values)); // 将key和对应的value进行reduce
        }

        // 写入文件
        Utils::write_file_content(outPutFile, result, chfs_client);
    }
}