#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include "static_sort.h" // 你的静态排序头文件（如果有）

using namespace std;

// 分区函数，用于快速选择算法
void partition(vector<int> &nums, int &low, int &high, int &pivot)
{
    for (int i = low; i != high;)
    {
        if (nums[i] > nums[pivot])
        {
            swap(nums[i], nums[--high]);
        }
        else
        {
            if (nums[i] < nums[pivot])
                swap(nums[i], nums[low++]);
            ++i;
        }
    }
    swap(nums[pivot], nums[high++]);
}

// 快速选择算法
int quickSelect(vector<int> &nums, int k)
{
    int n = nums.size();
    for (int lo = 0, hi = n - 1; lo < hi;)
    {
        int i = lo, j = hi;
        int pivot = nums[lo];
        while (i < j)
        {
            while (i < j && nums[j] >= pivot)
                j--;
            nums[i] = nums[j];
            while (i < j && nums[i] <= pivot)
                i++;
            nums[j] = nums[i];
        }
        nums[i] = pivot;
        if (k <= i)
            hi = i - 1;
        if (i <= k)
            lo = i + 1;
    }
    return nums[k];
}

// 线性选择算法
int linearSelect(vector<int> &nums, int begin, int end, int k, unsigned Q)
{
    // 递归基
    if (end - begin < Q){
        sort(nums.begin() + begin, nums.begin() + end);
        return begin + k;
    }
    // 划分数据集&选择中位数的中位数
    {   int i = begin, j = begin;
        unsigned n = (end - begin) / Q;
        while (n--){
            sort(nums.begin() + i, nums.begin() + i + Q);
            swap(nums[j++], nums[i + Q / 2]);
            i += Q;
        }
        if (i != end){
            sort(nums.begin() + i, nums.begin() + end);
            swap(nums[j++], nums[i + (end - i) / 2]);
        }
        int pivot = linearSelect(nums, begin, j, (j - begin) / 2, Q);
        swap(nums[end - 1], nums[pivot]);
    }
    // 划分为三个子集 减小规模
    {   int low = begin, high = end - 1, pivot = high;
        partition(nums, low, high, pivot);
        int l = low - begin, le = high - begin;
        if (l <= k && k < le)
            return begin + k;
        else if (k < l)
            return linearSelect(nums, begin, low, k, Q);
        else
            return linearSelect(nums, high, end, k - le, Q);
    }
}

// 测试函数
void testAlgorithms(int dataSize, unsigned Q, ofstream &outputFile, int numTests)
{
    // 生成顺序数据集
    vector<int> nums_ordered(dataSize);
    for (int i = 0; i < dataSize; ++i)
    {
        nums_ordered[i] = i; // 生成0到dataSize-1的顺序数
    }

    // 生成逆序数据集
    vector<int> nums_reverse(dataSize);
    for (int i = 0; i < dataSize; ++i)
    {
        nums_reverse[i] = dataSize - i - 1; // 生成dataSize-1到0的逆序数
    }

    // 生成乱序数据集
    vector<int> nums_random(dataSize);
    srand(time(nullptr));
    for (int i = 0; i < dataSize; ++i)
    {
        nums_random[i] = rand() % dataSize; // 生成0到dataSize-1之间的随机数
    }

    // 测试Quick Select算法
    auto testQuickSelect = [&](vector<int> nums, const string &dataLabel)
    {
        double totalQuickSelectTime = 0.0;
        for (int i = 0; i < numTests; ++i)
        {
            clock_t start = clock();
            int quickSelected = quickSelect(nums, dataSize / 2);
            clock_t end = clock();
            totalQuickSelectTime += double(end - start) / CLOCKS_PER_SEC;
        }
        double averageQuickSelectTime = totalQuickSelectTime / numTests;

        // 输出结果到文件
        outputFile << dataLabel << ",Quick Select," << dataSize << "," << averageQuickSelectTime << endl;
    };

    // 测试Linear Select算法
    auto testLinearSelect = [&](vector<int> nums, const string &dataLabel)
    {
        double totalLinearSelectTime = 0.0;
        for (int i = 0; i < numTests; ++i)
        {
            clock_t start = clock();
            int linearSelected = linearSelect(nums, 0, dataSize, dataSize / 2, Q); // 选择中位数
            clock_t end = clock();
            totalLinearSelectTime += double(end - start) / CLOCKS_PER_SEC;
        }
        double averageLinearSelectTime = totalLinearSelectTime / numTests;

        // 输出结果到文件
        outputFile << dataLabel << ",Linear Select," << dataSize << "," << averageLinearSelectTime << endl;
    };

    // 进行多次测试
    
        testQuickSelect(nums_ordered, "Ordered");
        testQuickSelect(nums_reverse, "Reverse");
        testQuickSelect(nums_random, "Random");

        testLinearSelect(nums_ordered, "Ordered");
        testLinearSelect(nums_reverse, "Reverse");
        testLinearSelect(nums_random, "Random");
    
}

void testLinearSelect(int dataSize, unsigned Q, ofstream &outputFile, int numTests)
{
    // 进行多次测试并计算平均时间
    double totalLinearSelectTime = 0.0;
    for (int i = 0; i < numTests; ++i)
    {
        // 生成随机数据集
        vector<int> nums(dataSize);
        srand(time(nullptr));
        for (int i = 0; i < dataSize; ++i)
        {
            nums[i] = rand() % dataSize; // 生成0到dataSize-1之间的随机数
        }

        // 测试Linear Select算法
        clock_t start = clock();
        linearSelect(nums, 0, dataSize, dataSize / 2, Q); // 选择中位数
        clock_t end = clock();
        totalLinearSelectTime += double(end - start) / CLOCKS_PER_SEC;
    }
    double averageLinearSelectTime = totalLinearSelectTime / numTests;

    // 输出结果到文件
    outputFile << dataSize << "," << Q << "," << averageLinearSelectTime << endl;
}

int main()
{
    ofstream outputFile("algorithm_performance.csv");

    // 写入表头
    outputFile << "Data Type,Algorithm,Data Size,Time (seconds)" << endl;

    // 测试不同规模和参数的数据集
    for (int dataSize = 10000; dataSize <= 100000; dataSize += 10000)
    {
        testAlgorithms(dataSize, 7, outputFile, 10); // 使用Q=7进行测试
    }

    // 线性选择算法性能随Q的变化测试
    vector<int> dataSizes = {1000, 10000, 100000};
    for (int dataSize : dataSizes)
    {
        for (unsigned Q = 2; Q <= 20; ++Q)
        {
            testLinearSelect(dataSize, Q, outputFile, 10); // 测试不同Q值对算法性能的影响
        }
    }

    outputFile.close();

    return 0;
}