// implement a radix tree that support node compressino and store int32_t values
// each parent node has 4 children, representing 2 bits
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <queue>
#include "tree.hpp"

struct compressed_node
{
    compressed_node *child[4]; // 00, 01, 10, 11
    compressed_node *parent;
    bool is_leaf;
    int32_t prefix; // only valid when length>0
    int length;     // length of the prefix
    compressed_node(compressed_node *parent = nullptr, bool is_leaf = false, int32_t prefix = 0, int length = 0)
    {
        for (int i = 0; i < 4; i++)
        {
            child[i] = nullptr;
        }
        this->parent = parent;
        this->is_leaf = is_leaf;
        this->prefix = prefix;
        this->length = length;
    }
};

class CompressedRadixTree : public Tree
{
private:
    compressed_node *root;
    int size_;

public:
    CompressedRadixTree();
    ~CompressedRadixTree();
    // basic operation
    void insert(int32_t value);
    void erase(int32_t value);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};