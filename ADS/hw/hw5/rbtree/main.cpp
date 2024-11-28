#include <iostream>
#include "rbtree.h"

void testOrdered()
{
    // Test ordered logic
    RedBlackTree tree;
    for (int i = 0; i < 10000; i++)
    {
        tree.insert(i);
    }
    std::cout << "fixTime: " << tree.fixTime << std::endl;
    std::cout << "rotateTime: " << tree.rotateTime << std::endl;
    std::cout << "colorTime: " << tree.colorTime << std::endl;
}

void testUnordered(int n)
{
    int totalFixTime = 0;
    int totalRotateTime = 0;
    int totalColorTime = 0;

    for (int test = 0; test < n; ++test)
    {
        RedBlackTree tree;
        int a[10000];

        // 初始化数组
        for (int i = 0; i < 10000; i++)
        {
            a[i] = i;
        }

        // 使用不同的随机种子
        srand(time(nullptr) + test);

        // 随机打乱数组
        for (int i = 0; i < 10000; i++)
        {
            int j = rand() % 10000;
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }

        // 乱序插入
        for (int i = 9999; i >= 0; i--)
        {
            tree.insert(a[i]);
        }

        // 统计每次测试的 fixTime、rotateTime 和 colorTime
        totalFixTime += tree.fixTime;
        totalRotateTime += tree.rotateTime;
        totalColorTime += tree.colorTime;
    }

    // 输出总体统计结果
    std::cout << "Average fixTime: " << static_cast<double>(totalFixTime) / n << std::endl;
    std::cout << "Average rotateTime: " << static_cast<double>(totalRotateTime) / n << std::endl;
    std::cout << "Average colorTime: " << static_cast<double>(totalColorTime) / n << std::endl;
}

int main()
{
    RedBlackTree tree;

    tree.insert(7);
    tree.insert(3);
    tree.insert(18);
    tree.insert(10);
    tree.insert(22);
    tree.insert(8);
    tree.insert(11);
    tree.insert(26);

    std::cout << "Inorder traversal of the constructed tree: \n";
    tree.inorder();
    std::cout << std::endl;

    std::cout << "Test ordered: \n";
    testOrdered();
    std::cout << std::endl;
    std::cout << "Test unordered: \n";
    testUnordered(3000);
    std::cout << std::endl;
    return 0;
}
