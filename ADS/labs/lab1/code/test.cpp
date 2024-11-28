#include <iostream>
#include <fstream>
#include <iomanip> // For setw()
#include <random>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cmath>
#include "radix_tree.hpp"            // 假设你的基数树的头文件
#include "compressed_radix_tree.hpp" // 假设你的压缩基数树的头文件
#include <set>                       // 假设你的红黑树的头文件
#include "util.hpp"                  // 包含获取满足 Zipfian 分布数字的工具函数

// 定义工作负载
enum class Workload
{
    Find,
    Insert,
    Delete
};

// 定义基本操作
template <typename Tree>
std::chrono::duration<double> perform_operation(Tree *tree, Workload operation, int value)
{
    std::chrono::time_point<std::chrono::steady_clock> start_op_time;
    std::chrono::time_point<std::chrono::steady_clock> end_op_time;
    switch (operation)
    {
    case Workload::Find:
        start_op_time = std::chrono::steady_clock::now();
        tree->find(value);
        end_op_time = std::chrono::steady_clock::now();
        return (end_op_time - start_op_time);
    case Workload::Insert:
        start_op_time = std::chrono::steady_clock::now();
        tree->insert(value);
        end_op_time = std::chrono::steady_clock::now();
        return (end_op_time - start_op_time);
    case Workload::Delete:
        start_op_time = std::chrono::steady_clock::now();
        tree->erase(value);
        end_op_time = std::chrono::steady_clock::now();
        return (end_op_time - start_op_time);
    }
    return std::chrono::duration<double>(0);
}

// 运行测试
template <typename Tree>
void run_tests(Tree *tree, const std::vector<double> &operation_probabilities, int duration, std::ofstream &file)
{
    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(duration);

    std::vector<int> zipf_data(1000, 1); // 存储 Zipf 分布的数据集

    std::vector<double> operation_times; // 存储操作时延
    std::vector<double> find_times;
    std::vector<double> insert_times;
    std::vector<double> delete_times;

    std::default_random_engine generator;
    std::discrete_distribution<int> distribution(operation_probabilities.begin(), operation_probabilities.end());
    // 重新生成 Zipf 分布的数据集

    for (int i = 0; i < 1000; ++i)
    {
        int value = zipf(); // 使用工具函数生成 Zipfian 分布的数字
        zipf_data[i] = value;
        tree->insert(value); // 向树中插入数据
    }

    while (std::chrono::steady_clock::now() < end_time)
    {
        int op_index = distribution(generator); // 使用概率分布选择操作
        int value = zipf();                     // 使用工具函数生成 Zipfian 分布的数字

        std::chrono::duration<double> elapsed_seconds = perform_operation(tree, static_cast<Workload>(op_index), value);
        operation_times.push_back(elapsed_seconds.count()); // 记录操作时延
        switch (static_cast<Workload>(op_index))            // 记录不同操作的时延
        {
        case Workload::Find:
            find_times.push_back(elapsed_seconds.count());
            break;
        case Workload::Insert:
            insert_times.push_back(elapsed_seconds.count());
            break;
        case Workload::Delete:
            delete_times.push_back(elapsed_seconds.count());
            break;
        }
    }

    // 计算 P50、P90、P99 时延
    std::sort(operation_times.begin(), operation_times.end());
    std::sort(find_times.begin(), find_times.end());
    std::sort(insert_times.begin(), insert_times.end());
    std::sort(delete_times.begin(), delete_times.end());

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << std::endl;
        return;
    }
    if (typeid(*tree) == typeid(RadixTree))
    {
        file << "      RadixTree Performance Metrics\n";
    }
    else if (typeid(*tree) == typeid(CompressedRadixTree))
    {
        file << "      CompressedRadixTree Performance Metrics\n";
    }
    else if (typeid(*tree) == typeid(std::set<int>))
    {
        file << "      RB-Tree Performance Metrics\n";
    }
    file << " Operation Type        Average Latency  P50 Latency  P90 Latency  P99 Latency \n";
    // Find 操作
    if (!find_times.empty())
    {
        int p50_index = std::round(0.50 * find_times.size());
        int p90_index = std::round(0.90 * find_times.size());
        int p99_index = std::round(0.99 * find_times.size());
        double find_avg = std::accumulate(find_times.begin(), find_times.end(), 0.0) / find_times.size();

        file << " Total Find            " << std::setw(15) << std::scientific << find_avg << "s";
        file << "  " << std::setw(11) << std::scientific << find_times[p50_index] << "s";
        file << "  " << std::setw(11) << std::scientific << find_times[p90_index] << "s";
        file << "  " << std::setw(11) << std::scientific << find_times[p99_index] << "s \n";
    }

    // Insert 操作
    if (!insert_times.empty())
    {
        int p50_index = std::round(0.50 * insert_times.size());
        int p90_index = std::round(0.90 * insert_times.size());
        int p99_index = std::round(0.99 * insert_times.size());
        double insert_avg = std::accumulate(insert_times.begin(), insert_times.end(), 0.0) / insert_times.size();

        file << " Total Insert          " << std::setw(15) << std::scientific << insert_avg << "s";
        file << "  " << std::setw(11) << std::scientific << insert_times[p50_index] << "s";
        file << "  " << std::setw(11) << std::scientific << insert_times[p90_index] << "s";
        file << "  " << std::setw(11) << std::scientific << insert_times[p99_index] << "s \n";
    }

    // Delete 操作
    if (!delete_times.empty())
    {
        int p50_index = std::round(0.50 * delete_times.size());
        int p90_index = std::round(0.90 * delete_times.size());
        int p99_index = std::round(0.99 * delete_times.size());
        double delete_avg = std::accumulate(delete_times.begin(), delete_times.end(), 0.0) / delete_times.size();

        file << " Total Delete          " << std::setw(15) << std::scientific << delete_avg << "s";
        file << "  " << std::setw(11) << std::scientific << delete_times[p50_index] << "s";
        file << "  " << std::setw(11) << std::scientific << delete_times[p90_index] << "s";
        file << "  " << std::setw(11) << std::scientific << delete_times[p99_index] << "s \n";
    }

    // 总体操作
    if (!operation_times.empty())
    {
        int p50_index = std::round(0.50 * operation_times.size());
        int p90_index = std::round(0.90 * operation_times.size());
        int p99_index = std::round(0.99 * operation_times.size());
        double operation_avg = std::accumulate(operation_times.begin(), operation_times.end(), 0.0) / operation_times.size();

        file << " Total Operation       " << std::setw(15) << std::scientific << operation_avg << "s";
        file << "  " << std::setw(11) << std::scientific << operation_times[p50_index] << "s";
        file << "  " << std::setw(11) << std::scientific << operation_times[p90_index] << "s";
        file << "  " << std::setw(11) << std::scientific << operation_times[p99_index] << "s \n";
    }
}

int main()
{

    // 定义每种工作负载中各个操作的概率分布
    std::vector<double> operation_probabilities_1 = {0.5, 0.5, 0};     // 查找操作概率为50%，插入操作概率为50%
    std::vector<double> operation_probabilities_2 = {1, 0, 0};         // 查找操作概率为100%
    std::vector<double> operation_probabilities_3 = {0.5, 0.25, 0.25}; // 查找、插入和删除操作概率均等

    // 定义测试持续时间
    int duration = 60; // 单位：秒
    std::ofstream file("output.txt");

    // 运行测试
    std::cout << "Running workload 1 tests...\n";
    run_tests(new RadixTree(), operation_probabilities_1, duration, file);
    run_tests(new CompressedRadixTree(), operation_probabilities_1, duration, file);
    run_tests(new std::set<int>(), operation_probabilities_1, duration, file);

    std::cout << "Running workload 2 tests...\n";
    run_tests(new RadixTree(), operation_probabilities_2, duration, file);
    run_tests(new CompressedRadixTree(), operation_probabilities_2, duration, file);
    run_tests(new std::set<int>(), operation_probabilities_2, duration, file);

    std::cout << "Running workload 3 tests...\n";
    run_tests(new RadixTree(), operation_probabilities_3, duration, file);
    run_tests(new CompressedRadixTree(), operation_probabilities_3, duration, file);
    run_tests(new std::set<int>(), operation_probabilities_3, duration, file);

    return 0;
}
