#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

// 递归计算斐波那契数列
inline int fibonacci(int n, int threadNum)
{
    if (n <= 1)
    {
        return n;
    }
    if (threadNum > 1)
    {
        int x, y;
        std::thread t1([&]
                       { x = fibonacci(n - 1, threadNum / 2); });
        std::thread t2([&]
                       { y = fibonacci(n - 2, threadNum / 2); });
        t1.join();
        t2.join();
        return x + y;
    }
    else
    {
        return fibonacci(n - 1, threadNum) + fibonacci(n - 2, threadNum);
    }
}

int main()
{
    // 测试数据，计算斐波那契数列的第40个数
    int n = 40;

    // 测试多线程情况，分别使用2、4、8、16个线程
    std::vector<int> threadNums = {2, 4, 8, 16};

    // 单线程测试，共测试100次，取平均值，作为基准对比
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    double singleAverageTime = 0;
    int singleResult = 0;
    for (int i = 0; i < 10; i++)
    {
        start = std::chrono::high_resolution_clock::now();
        singleResult = fibonacci(n, 1);
        end = std::chrono::high_resolution_clock::now();
        singleAverageTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
    singleAverageTime /= 10;
    std::cout << "Using 1 thread: " << singleResult << " time taken: " << singleAverageTime << " milliseconds" << std::endl;

    // 做多线程测试，一共测试4种情况，每种情况测试100次，取平均值
    for (int threadNum : threadNums)
    {
        double averageTime = 0;
        int result = 0;
        for (int i = 0; i < 10; i++)
        {
            start = std::chrono::high_resolution_clock::now();
            result = fibonacci(n, threadNum);
            end = std::chrono::high_resolution_clock::now();
            averageTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
        averageTime /= 10;
        std::cout << "Using " << threadNum << " threads: " << result << " time taken: " << averageTime << " milliseconds" << std::endl;
    }
    return 0;
}