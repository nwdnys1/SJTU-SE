#include "radix_tree.hpp"

RadixTree::RadixTree()
{
    root = new radix_tree_node();
    size_ = 1;
}

void destroy_radix_tree(radix_tree_node *node)
{
    if (node == nullptr)
        return;
    for (int i = 0; i < 4; i++)
        destroy_radix_tree(node->child[i]);
    delete node;
}

RadixTree::~RadixTree()
{
    destroy_radix_tree(root);
}

void RadixTree::insert(int32_t value)
{
    radix_tree_node *tmp = root;
    for (int i = 30; i >= 0; i -= 2)
    {
        int index = (value >> i) & 0x3; // 00, 01, 10, 11
        if (tmp->child[index] == nullptr)
        {
            tmp->child[index] = new radix_tree_node();
            size_++;
            tmp->child[index]->parent = tmp;
        }
        tmp = tmp->child[index];
    }
}

int child_index(radix_tree_node *node)
{
    for (int i = 0; i < 4; i++)
    {
        if (node->parent->child[i] == node)
        {
            return i;
        }
    }
    return -1;
}

bool is_leaf(radix_tree_node *node)
{
    for (int i = 0; i < 4; i++)
    {
        if (node->child[i] != nullptr)
        {
            return false;
        }
    }
    return true;
}

void RadixTree::erase(int32_t value)
{
    radix_tree_node *tmp = root;
    for (int i = 30; i >= 0; i -= 2)
    {
        int index = (value >> i) & 0x3; // 00, 01, 10, 11
        if (tmp->child[index] == nullptr)
        {
            return;
        }
        tmp = tmp->child[index];
    }
    while (tmp->parent != nullptr)
    {
        int index = child_index(tmp);
        tmp->parent->child[index] = nullptr;
        delete tmp;
        size_--;
        tmp = tmp->parent;
        if (!is_leaf(tmp))
        {
            break;
        }
    }
}

bool RadixTree::find(int32_t value)
{
    radix_tree_node *tmp = root;
    for (int i = 30; i >= 0; i -= 2)
    {
        int index = (value >> i) & 0x3; // 00, 01, 10, 11
        if (tmp->child[index] == nullptr)
        {
            return false;
        }
        tmp = tmp->child[index];
    }
    return true;
}

uint32_t RadixTree::size()
{
    return size_;
}

uint32_t RadixTree::height()
{
    return is_leaf(root) ? 1 : 17;
}
