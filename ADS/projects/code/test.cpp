#include <iostream>
#include <cstdint>
#include <string>
#include <assert.h>
#include <fstream>

#include "test.h"

std::fstream file;

void test1(KVStore &kv)
{
    // 测试 put、get、scan 和 del 操作的性能
    kv.reset();
    for (int num_operations = 20000; num_operations <= 80000; num_operations += 20000)
    {

        // 记录 put 操作的总时间
        auto start_total_put = std::chrono::high_resolution_clock::now();
        double put_time = 0.0;

        for (int k = 1; k <= num_operations; ++k)
        {
            auto start_time = std::chrono::high_resolution_clock::now();

            // 执行 put 操作
            kv.put(k, "test!");

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            put_time += duration.count();
        }

        auto end_total_put = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> put_total_time = end_total_put - start_total_put;

        // 记录 get 操作的总时间
        auto start_total_get = std::chrono::high_resolution_clock::now();
        double get_time = 0.0;

        for (int k = 1; k <= num_operations; ++k)
        {
            auto start_time = std::chrono::high_resolution_clock::now();

            // 执行 get 操作
            kv.get(k);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            get_time += duration.count();
        }

        auto end_total_get = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> get_total_time = end_total_get - start_total_get;

        // 记录 scan 操作的总时间
        auto start_total_scan = std::chrono::high_resolution_clock::now();
        double scan_time = 0.0;
        scan_result_t scan_result;

        for (int k = 1; k <= num_operations; ++k)
        {
            auto start_time = std::chrono::high_resolution_clock::now();

            // 执行 scan 操作
            kv.scan(1, 10, scan_result);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            scan_time += duration.count();
        }

        auto end_total_scan = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> scan_total_time = end_total_scan - start_total_scan;

        // 记录 del 操作的总时间
        auto start_total_del = std::chrono::high_resolution_clock::now();
        double del_time = 0.0;

        for (int k = 1; k <= num_operations; ++k)
        {
            auto start_time = std::chrono::high_resolution_clock::now();

            // 执行 del 操作
            kv.del(k);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            del_time += duration.count();
        }

        auto end_total_del = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> del_total_time = end_total_del - start_total_del;

        // 输出 put、get、scan 和 del 操作的平均延迟和吞吐量
        double average_put_latency = put_time / num_operations;
        double average_get_latency = get_time / num_operations;
        double average_scan_latency = scan_time / num_operations;
        double average_del_latency = del_time / num_operations;

        double put_throughput = num_operations / put_total_time.count();
        double get_throughput = num_operations / get_total_time.count();
        double scan_throughput = num_operations / scan_total_time.count();
        double del_throughput = num_operations / del_total_time.count();

        file << "Number of operations: " << num_operations << "\n";

        file << "Average latency for " << " put operations: " << average_put_latency << " microseconds\n";
        file << "Throughput for " << " put operations: " << put_throughput << " operations per second\n";

        file << "Average latency for " << " get operations: " << average_get_latency << " microseconds\n";
        file << "Throughput for " << " get operations: " << get_throughput << " operations per second\n";

        file << "Average latency for " << " scan operations: " << average_scan_latency << " microseconds\n";
        file << "Throughput for " << " scan operations: " << scan_throughput << " operations per second\n";

        file << "Average latency for " << " del operations: " << average_del_latency << " microseconds\n";
        file << "Throughput for " << " del operations: " << del_throughput << " operations per second\n\n";
    }
}

void test2(KVStore &kv)
{
    // 测试不同的缓存策略对 get 操作的性能的影响

    kv.reset();

    int num_operations = 100;
    int data_size = 1000;

    // 插入 1000 个键值对
    for (int k = 1; k <= data_size; ++k)
        kv.put(k, "test!");

    // 记录 get 操作的总时间
    auto start_total_get = std::chrono::high_resolution_clock::now();
    double get_time = 0.0;

    for (int i = 0; i < num_operations; i++)
    {
        for (int k = 1; k <= data_size; ++k)
        {
            auto start_time = std::chrono::high_resolution_clock::now();

            // 执行 get 操作
            kv.get(k);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            get_time += duration.count();
        }
    }

    auto end_total_get = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> get_total_time = end_total_get - start_total_get;
    // 输出 get 操作的平均延迟和吞吐量
    double average_get_latency = get_time / num_operations / data_size;
    double get_throughput = num_operations * data_size / get_total_time.count();

    file << "Cache policy: " << cache_policy << "\n";
    file << "Average latency for " << " get operations: " << average_get_latency << " microseconds\n";
    file << "Throughput for " << " get operations: " << get_throughput << " operations per second\n\n";
}
void test3(KVStore &kv)
{
    // 测试随着插入操作次数的增加，put 操作的延迟的变化
    kv.reset();
    int num_operations = 4000;
    std::vector<double> put_time;
    double put_time_sum = 0.0;
    for (int k = 1; k <= num_operations; ++k)
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        kv.put(k, std::string(1000, 't'));
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        put_time_sum += duration.count();
        if (k % 100 == 0)
        {
            put_time.push_back(put_time_sum / 100);
            put_time_sum = 0.0;
        }
    }
    for (auto i : put_time)
    {
        file << i << " ";
    }
    file << std::endl;
}
void test4(KVStore &kv)
{
    kv.reset();
    int num_operations = 100000;

    // 记录 put 操作的总时间
    auto start_total_put = std::chrono::high_resolution_clock::now();
    double put_time = 0.0;

    for (int k = 1; k <= num_operations; ++k)
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        // 执行 put 操作
        kv.put(k, "test!");

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        put_time += duration.count();
    }

    auto end_total_put = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> put_total_time = end_total_put - start_total_put;

    // 记录 get 操作的总时间
    auto start_total_get = std::chrono::high_resolution_clock::now();
    double get_time = 0.0;

    for (int k = 1; k <= num_operations; ++k)
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        // 执行 get 操作
        kv.get(k);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        get_time += duration.count();
    }

    auto end_total_get = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> get_total_time = end_total_get - start_total_get;

    // 输出 put、get的平均延迟和吞吐量
    double average_put_latency = put_time / num_operations;
    double average_get_latency = get_time / num_operations;

    double put_throughput = num_operations / put_total_time.count();
    double get_throughput = num_operations / get_total_time.count();

    file << "Bloom filter size: " << sst_bf_size << "\n";

    file << "Average latency for " << " put operations: " << average_put_latency << " microseconds\n";
    file << "Throughput for " << " put operations: " << put_throughput << " operations per second\n";

    file << "Average latency for " << " get operations: " << average_get_latency << " microseconds\n";
    file << "Throughput for " << " get operations: " << get_throughput << " operations per second\n\n";
}
void example()
{
    KVStore store("./data");
    store.reset();
    store.put(1, "1");
    store.get(1);
    scan_result_t result;
    store.scan(1, 1, result);
    store.del(1);
    store.gc(16 * MB);
}

int main()
{
    file.open("./test_result.txt", std::ios::out);
    KVStore store("./data");
    file << "test1" << std::endl;
    test1(store);
    file << "test2" << std::endl;
    test2(store);
    file << "test3" << std::endl;
    test3(store);
    file << "test4" << std::endl;
    test4(store);
    return 0;
}