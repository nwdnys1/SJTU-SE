#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include "src/hnsw.hpp"
#include "util/vecs_io.hpp"
#include "util/ground_truth.hpp"
#include "util/parameter.hpp"

using namespace std;
using namespace HNSWLab;

HNSW hnsw;
HNSW hnsw_parallel;
int *query;
int query_n_vec;
int query_vec_dim;
int thread_num = 8;
int query_num = 10;
std::vector<std::thread> threads(thread_num); // 定义存储线程对象的容器
vector<vector<int>> test_gnd_l(100);          // 存储查询结果用于计算recall

void query_tasks(int id)
{
    int start_index = id * query_n_vec / thread_num;
    int end_index = std::min((id + 1) * query_n_vec / thread_num, query_n_vec);
    for (int i = start_index; i < end_index; ++i)
    {
        vector<int> test_gnd = hnsw_parallel.query(query + i * query_vec_dim, query_num); // 执行查询操作
        test_gnd_l[i] = test_gnd;
    }
}

void test_sequential()
{
    std::printf("load query\n");
    query_n_vec = 100;
    query_vec_dim = 128;
    char *path = "./data/siftsmall/query.bvecs";
    query = read_bvecs(query_n_vec, query_vec_dim, path);

    std::printf("load base\n");
    int base_n_vec = 10000;
    int base_vec_dim = 128;
    path = "./data/siftsmall/base.bvecs";
    int *base = read_bvecs(base_n_vec, base_vec_dim, path);

    // 进行插入时间测试 - HNSW
    std::chrono::duration<double, std::micro> insert_elapsed_hnsw(0);
    auto insert_start_hnsw = std::chrono::steady_clock::now();
    for (int i = 0; i < base_n_vec; i++)
        hnsw.insert(base + base_vec_dim * i, i);

    auto insert_end_hnsw = std::chrono::steady_clock::now();
    insert_elapsed_hnsw = insert_end_hnsw - insert_start_hnsw;
    double average_insert_time_hnsw = insert_elapsed_hnsw.count() / base_n_vec;

    // 进行查询时间测试 - HNSW
    std::chrono::duration<double, std::micro> query_elapsed_hnsw(0);
    auto query_start_hnsw = std::chrono::steady_clock::now();
    for (int i = 0; i < query_n_vec; i++)
        hnsw.query(query + i * query_vec_dim, query_num);

    auto query_end_hnsw = std::chrono::steady_clock::now();
    query_elapsed_hnsw = query_end_hnsw - query_start_hnsw;
    double average_query_time_hnsw = query_elapsed_hnsw.count() / query_n_vec;

    // 输出结果
    std::cout << "Average insertion time (HNSW): " << average_insert_time_hnsw << " microseconds" << std::endl;
    std::cout << "Average query time (HNSW): " << average_query_time_hnsw << " microseconds" << std::endl;
}

void test_parallel()
{
    std::printf("load ground truth\n");
    int gnd_n_vec = 100;
    int gnd_vec_dim = 10;
    char *path = "./data/siftsmall/gnd.ivecs";
    int *gnd = read_ivecs(gnd_n_vec, gnd_vec_dim, path);

    std::printf("load query\n");
    query_n_vec = 100;
    query_vec_dim = 128;
    path = "./data/siftsmall/query.bvecs";
    query = read_bvecs(query_n_vec, query_vec_dim, path);

    std::printf("load base\n");
    int base_n_vec = 10000;
    int base_vec_dim = 128;
    path = "./data/siftsmall/base.bvecs";
    int *base = read_bvecs(base_n_vec, base_vec_dim, path);

    // 进行插入 - HNSW_Parallel
    for (int i = 0; i < base_n_vec; i++)
        hnsw_parallel.insert(base + base_vec_dim * i, i);
    // 进行查询时间测试 - HNSW_Parallel

    std::chrono::duration<double, std::micro> query_elapsed_parallel(0);
    auto query_start_parallel = std::chrono::steady_clock::now();
    for (int i = 0; i < thread_num; i++)
        threads[i] = std::thread(query_tasks, i);
    for (int i = 0; i < thread_num; i++) // 等待所有线程结束
        threads[i].join();
    auto query_end_parallel = std::chrono::steady_clock::now();
    query_elapsed_parallel = query_end_parallel - query_start_parallel;
    double average_query_time_parallel = query_elapsed_parallel.count() / query_n_vec;
    // 输出结果
    double recall = count_recall(gnd_n_vec, gnd_vec_dim, test_gnd_l, gnd);
    std::cout << "average recall(Parallel): " << recall << std::endl;
    std::cout << "Average query time (Parallel): " << average_query_time_parallel << " microseconds" << std::endl;
}

int main()
{

    test_sequential();
    test_parallel();
    return 0;
}
