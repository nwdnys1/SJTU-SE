
#include <cstdint>
#include <iostream>
#include "tree.hpp"

struct radix_tree_node
{
    radix_tree_node *child[4]; // 00, 01, 10, 11
    radix_tree_node *parent;
    radix_tree_node()
    {
        for (int i = 0; i < 4; i++)
        {
            child[i] = nullptr;
        }
    }
};

class RadixTree : public Tree
{
    // To Be Implemented
private:
    radix_tree_node *root;
    int size_;

public:
    RadixTree();
    ~RadixTree();
    // basic operation
    void insert(int32_t value);
    void erase(int32_t value);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};
