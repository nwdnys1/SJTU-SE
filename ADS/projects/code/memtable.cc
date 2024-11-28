#include "memtable.h"
#include "sstable.h"

void Memtable::put(key_type key, const value_t &value)
{
    int new_level = random_level();
    if (new_level > level) // 如果新节点的层数大于当前跳表的层数，需要更新head的next
    {
        for (int i = level; i < new_level; i++)
        {
            head->next[i] = nullptr;
        }
        level = new_level;
    }
    node *cur = head;
    std::vector<node *> update(level, nullptr);
    for (int i = level - 1; i >= 0; i--)
    {
        while (cur->next[i] != nullptr && cur->next[i]->key < key)
        {
            cur = cur->next[i];
        }
        update[i] = cur;
    }
    // 如果key已经存在，直接更新value
    if (cur->next[0] != nullptr && cur->next[0]->key == key)
    {
        cur->next[0]->val = value;
        // 因为不计算value的大小 一个元组的更新不会改变size
        return;
    }
    // 否则插入新节点
    node *new_node = new node(key, value, new_level);
    for (int i = 0; i < new_level; i++)
    {
        new_node->next[i] = update[i]->next[i];
        update[i]->next[i] = new_node;
    }
    size += sizeof(Tuple::key) + sizeof(Tuple::offset) + sizeof(Tuple::vlen); // 一个tuple的大小 不包括value
}

value_t Memtable::get(key_type key)
// 一个不存在的值可能有两种情况 1.不存在 2.被删除 1的情况直接返回空字符串 2的情况返回delete_tag
{
    node *cur = head;
    for (int i = level - 1; i >= 0; i--)
    {
        while (cur->next[i] != nullptr && cur->next[i]->key < key)
        {
            cur = cur->next[i];
        }
        if (cur->next[i] != nullptr && cur->next[i]->key == key)
        {
            return cur->next[i]->val; // 存在的值 但是可能是被删除的
        }
    }
    return ""; // 不存在的值
}

void Memtable::del(key_type key) // 内置的真删除
{
    node *cur = head;
    std::vector<node *> update(level, nullptr); // 记录每一层目标节点的前驱节点
    for (int i = level - 1; i >= 0; i--)
    {
        while (cur->next[i] != nullptr && cur->next[i]->key < key)
        {
            cur = cur->next[i];
        }
        update[i] = cur;
    }
    if (cur->next[0] != nullptr && cur->next[0]->key == key) // 找到目标节点
    {
        node *del_node = cur->next[0];
        for (int i = 0; i < level; i++)
        {
            if (update[i]->next[i] == del_node)
            {
                update[i]->next[i] = del_node->next[i];
            }
        }
        delete del_node;
    }
}
// 初始化 先清空跳表 再重新初始化head
void Memtable::reset()
{
    freeSkipList(head);
    head = new node(-1, "", max_level);
    level = 1;
    size = sst_header_size + sst_bf_size;
}

void Memtable::scan(key_type start, key_type end, std::map<uint64_t, std::string> &scan_result)
{

    node *cur = head;
    for (int i = level - 1; i >= 0; i--)
    {
        while (cur->next[i] != nullptr && cur->next[i]->key < start)
        {
            cur = cur->next[i];
        }
    }
    cur = cur->next[0];
    while (cur != nullptr && cur->key <= end)
    {
        if (cur->val != delete_tag) // 跳过被删除的节点
            scan_result[cur->key] = cur->val;
        else // 删除被删除的节点 可能来自sstable的结果 因为内存是最优先的
            scan_result.erase(cur->key);
        cur = cur->next[0];
    }
}
