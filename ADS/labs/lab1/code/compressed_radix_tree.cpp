#include "compressed_radix_tree.hpp"

CompressedRadixTree::CompressedRadixTree()
{
    root = new compressed_node();
    size_ = 1;
}

void destroy_compressed_radix_tree(compressed_node *node)
{
    if (node == nullptr)
        return;
    for (int i = 0; i < 4; i++)
        destroy_compressed_radix_tree(node->child[i]);
    delete node;
}

CompressedRadixTree::~CompressedRadixTree()
{
    destroy_compressed_radix_tree(root);
}

int32_t first_k_bits(int32_t value, int k)
{
    return k == 32 ? value : (value >> (32 - k)) & ((1 << k) - 1);
}

int32_t last_k_bits(int32_t value, int k)
{
    return k == 32 ? value : value & ((1 << k) - 1);
}

void CompressedRadixTree::insert(int32_t value)
{
    compressed_node *tmp = root;
    compressed_node *parent = nullptr;
    int i = 32; // 从最高位开始 代表了value还剩i位有效位
    while (i > 0)
    {
        int index = first_k_bits(value, 2); // 取出value的前两位
        if (tmp->child[index] == nullptr)   // 查找儿子 为空说明前缀匹配到此 需要插入新节点
        {
            compressed_node *new_node = new compressed_node(tmp, true, first_k_bits(value, i), i);
            size_++;
            tmp->child[index] = new_node;
            tmp->is_leaf = false; // 插入新节点后当前节点不再是叶子节点
            return;
        }
        // 前2位是匹配的
        tmp = tmp->child[index];
        int len = tmp->length;
        int old_prefix = tmp->prefix;
        int new_prefix = first_k_bits(value, i);
        int common_len = (__builtin_clz((old_prefix << (i - len)) ^ new_prefix) - 32 + i) / 2 * 2;
        if (old_prefix != new_prefix && common_len < len)                                                                                       // 前缀不匹配 公共前缀长度大于原来的前缀长度 需要分裂出一个儿子节点
        {                                                                                                                                       // 计算公共前缀长度 2的倍数
            compressed_node *split_parent = tmp;                                                                                                // 原来的节点前缀分裂成的父节点
            compressed_node *new_child = new compressed_node(tmp, true, last_k_bits(new_prefix, i - common_len), i - common_len);               // 新插入的节点
            compressed_node *split_child = new compressed_node(tmp, tmp->is_leaf, last_k_bits(old_prefix, len - common_len), len - common_len); // 分裂出的儿子节点
            if (!split_parent->is_leaf)                                                                                                         // 将原来的节点的儿子节点赋值给新的节点
                for (int j = 0; j < 4; j++)
                {
                    if (split_parent->child[j] != nullptr)
                    {
                        split_child->child[j] = split_parent->child[j];
                        split_parent->child[j]->parent = split_child;
                    }
                    split_parent->child[j] = nullptr;
                }
            split_parent->child[first_k_bits(last_k_bits(old_prefix, len - common_len) << (32 - len + common_len), 2)] = split_child;
            // 操作父亲
            split_parent->child[first_k_bits(new_prefix << 32 - i + common_len, 2)] = new_child;
            split_parent->prefix = first_k_bits(new_prefix, 32 - i + common_len);
            split_parent->length = common_len;
            split_parent->is_leaf = false;
            size_ += 2;
            return;
        }

        // 前缀匹配 如果是叶子节点说明找到了直接返回 否则继续向下查找
        if (tmp->is_leaf)
            return;
        i -= len;
        value <<= len;
    }
}

void CompressedRadixTree::erase(int32_t value)
{
    compressed_node *tmp = root;
    while (1) // 查找value的节点
    {
        int index = first_k_bits(value, 2); // 取出value的前两位
        if (tmp->child[index] == nullptr)   // 查找儿子 为空说明没有找到

            return;

        // 找到儿子
        tmp = tmp->child[index];
        int len = tmp->length;
        // 取出前len位进行比对
        if (tmp->prefix != first_k_bits(value, len))
            return;
        // 比对成功 如果是叶子节点说明找到了
        if (tmp->is_leaf)
            break;
        value <<= len;
    }
    // 删除节点
    compressed_node *parent = tmp->parent;
    int index = first_k_bits(value, 2);
    parent->child[index] = nullptr;
    delete tmp;
    size_--;
    // 删除节点后需要检查父节点是否需要合并
    int count = 0;
    compressed_node *child = nullptr;
    for (int i = 0; i < 4; i++)
    {
        if (parent->child[i] != nullptr)
        {
            count++;
            child = parent->child[i];
        }
    }
    if (count != 1) // 不需要合并
        return;

    // 合并
    int new_prefix = (parent->prefix << child->length) | child->prefix;
    int new_length = parent->length + child->length;
    for (int i = 0; i < 4; i++)
    {
        parent->child[i] = child->child[i];
        if (child->child[i])
            child->child[i]->parent = parent;
    }
    parent->prefix = new_prefix;
    parent->length = new_length;
    parent->is_leaf = child->is_leaf;
    delete child;
    size_--;
}

bool CompressedRadixTree::find(int32_t value)
{
    compressed_node *tmp = root;
    while (1)
    {
        int index = first_k_bits(value, 2); // 取出value的前两位
        if (tmp->child[index] == nullptr)   // 查找儿子 为空说明没有找到
        {
            return false;
        }
        // 找到儿子
        tmp = tmp->child[index];
        int len = tmp->length;
        // 取出前len位进行比对
        if (tmp->prefix != first_k_bits(value, len))
            return false;
        // 比对成功 如果是叶子节点说明找到了
        if (tmp->is_leaf)
            return true;
        value <<= len;
    }
    return false;
}

uint32_t CompressedRadixTree::size()
{
    // tranverse(root);
    return size_;
}

int node_height(compressed_node *node)
{
    if (node->is_leaf)
        return 1;
    int h = 0;
    for (int i = 0; i < 4; i++)
    {
        if (node->child[i] != nullptr)
            h = h > node_height(node->child[i]) ? h : node_height(node->child[i]); // 递归计算子节点的高度
    }
    return h + 1;
}

uint32_t CompressedRadixTree::height()
{
    return node_height(root);
}
