#include <iostream>
#include "base.hpp"
#include "../util/util.hpp"
#include <vector>
#include <cstring>

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <random>
#include <cassert>
#include <functional>
using namespace HNSWLab;
#define vec_t const int *
namespace HNSWLab
{
    struct node
    {
        vec_t vec; // vector
        int id;    // label
        std::function<bool(node *, node *)> less = [this](node *a, node *b)
        {
            return l2distance(a->vec, this->vec, 128) < l2distance(b->vec, this->vec, 128);
        };
        std::vector<std::vector<node *>> next;                                                 // next[i]是当前节点在第i层的邻接节点集合（指针）
        node(vec_t v, int i, int level) : vec(v), id(i), next(level, std::vector<node *>()){}; // 初始化为level层，每层都是空的vector
    };

    class HNSW : public AlgorithmInterface
    {
    private:
        node *enter_point = nullptr; // 入口点
        int max_level;               // 最大层数
    public:
        HNSW() {}
        void insert(vec_t item, int label);
        std::vector<int> query(vec_t query, int k);
        std::vector<node *> search_layer(vec_t q, node *ep, int ef, int lc); // 从入口点ep开始搜索某一层q节点的ef个邻居
        ~HNSW() {}
    };

    /**
     * input:
     * item: the vector to be inserted
     * label: the label(id) of the vector
     */
    void HNSW::insert(vec_t q, int label)
    {
        if (!enter_point) // 没有任何节点时，直接插入
        {
            int L = get_random_level();              // 随机生成层数
            enter_point = new node(q, label, L + 1); // 新节点
            max_level = L;                           // 更新最大层数
            return;
        }
        // 第一步
        auto greater = [&q](node *a, node *b)
        { return l2distance(a->vec, q, 128) > l2distance(b->vec, q, 128); };            // 128 维不知道是如何得出的
        std::priority_queue<node *, std::vector<node *>, decltype(greater)> W(greater); // 小根堆存储最近邻元素集合 方便取出
        node *ep = enter_point;                                                         // 入口点
        int maxL = max_level;                                                           // 最大层数
        int L = get_random_level();                                                     // 随机生成层数
        node *new_node = new node(q, label, L + 1);                                     // 新节点

        // 第二步（如果L>=maxL，会直接跳过第二步，相当于从ep开始搜索）
        for (int lc = maxL; lc > L; lc--) // 从最大层数开始，逐层搜索到L+1层
        {
            W.push(search_layer(q, ep, 1, lc)[0]); // 从入口点开始搜索当层的最近邻点并放入W
            ep = W.top();                          // 更新入口点为W中的最近邻点
        }

        // 第三步
        for (int lc = std::min(L, maxL); lc >= 0; lc--) // 从L层开始，逐层搜索到0层
        {
            std::vector<node *> v = search_layer(q, ep, ef_construction, lc);                                   // 当前层向量q最近邻的efc个点集合
            std::priority_queue<node *, std::vector<node *>, decltype(greater)> W(v.begin(), v.end(), greater); // 赋值给W建堆
            int cnt = std::min(M, (int)W.size());                                                               // 有可能不足M个点                                                                    // 取出最近邻的M个点或者全部点
            ep = W.top();                                                                                       // 从W中取出最近邻点作为下一层的入口点（现在取是因为后续要出堆）                                                              // 用于存储新节点的邻接节点
            while (cnt)                                                                                         // 将这些点在lc层和q建立连接
            {
                node *p = W.top();               // 取出堆顶元素
                W.pop();                         // 弹出堆顶元素
                new_node->next[lc].push_back(p); // 将堆顶元素加入new_node的邻接节点集合
                p->next[lc].push_back(new_node); // 将new_node加入堆顶元素的邻接节点集合
                if (p->next.size() > M_max)      // 如果邻接节点集合超过M_max，删除最远的点
                {
                    std::make_heap(p->next[lc].begin(), p->next[lc].end(), p->less); // 将p的邻接节点集合建立成大根堆 方便取出最远的点
                    int i = p->next.size() - M_max;
                    while (i--) // 这里只删除了单向的边 不懂要求是什么
                    {
                        std::pop_heap(p->next[lc].begin(), p->next[lc].end(), p->less); // 将最远的点放到最后
                        p->next[lc].pop_back();                                         // 删除最远的点
                    }
                }
                cnt--;
            }
        }

        // 如果L>maxL，将新节点作为入口点
        if (L > maxL)
        {
            enter_point = new_node;
            max_level = L;
        }
    }

    /**
     * input:
     * query: the vector to be queried
     * k: the number of nearest neighbors to be returned
     *
     * output:
     * a vector of labels of the k nearest neighbors
     */
    std::vector<int> HNSW::query(vec_t q, int k)
    {
        std::vector<int> res;
        auto greater = [&q](node *a, node *b)
        { return l2distance(a->vec, q, 128) > l2distance(b->vec, q, 128); };            // 128 维不知道是如何得出的
        std::priority_queue<node *, std::vector<node *>, decltype(greater)> W(greater); // 小根堆存储最近邻元素集合 方便取出
        node *ep = enter_point;                                                         // 入口点
        int maxL = max_level;                                                           // 最大层数

        // 第一步
        for (int lc = maxL; lc > 0; lc--) // 从最大层数开始，逐层搜索到1层
        {
            W.push(search_layer(q, ep, 1, lc)[0]); // 从入口点开始搜索当层的最近邻点并放入W
            ep = W.top();                          // 更新入口点为W中的最近邻点
        }

        // 第二步
        std::vector<node *> v = search_layer(q, ep, ef_construction, 0); // 从入口点开始搜索0层的efc个最近邻点
        std::make_heap(v.begin(), v.end(), greater);                     // 将v建立成小根堆
        while (k--)                                                      // 取出k个最近邻的标签
        {
            res.push_back(v.front()->id);               // 取出堆顶元素
            std::pop_heap(v.begin(), v.end(), greater); // 弹出堆顶元素
            v.pop_back();
        }

        return res;
    }
    /**
     * input:
     * q: the vector to be searched
     * ep: the entry point
     * ef: the number of neighbors to be searched
     * lc: the level to be searched
     *
     * output:
     * a vector of pointers to the ef nearest neighbors of q
     */
    std::vector<node *> HNSW::search_layer(vec_t q, node *ep, int ef, int lc)
    {
        auto greater = [&q](node *a, node *b) // 用于比较两个节点与q的距离
        { return l2distance(a->vec, q, 128) > l2distance(b->vec, q, 128); };
        auto less = [&q](node *a, node *b) // 用于比较两个节点与q的距离
        { return l2distance(a->vec, q, 128) < l2distance(b->vec, q, 128); };

        std::unordered_map<node *, bool> visited;    // 访问过的节点集合
        std::vector<node *> C;                       // 候选节点集合，一个小根堆，方便取出最近邻的点
        std::vector<node *> W;                       // 现在发现的最近邻节点集合，一个大根堆，方便取出最远的点
        visited[ep] = true;                          // 标记入口点为访问过
        C.push_back(ep);                             // 将入口点加入C
        W.push_back(ep);                             // 将入口点加入W
        std::make_heap(C.begin(), C.end(), greater); // 将C建立成小根堆
        std::make_heap(W.begin(), W.end(), less);    // 将W建立成大根堆
        node *c, *f;                                 // C中的最近邻和W中的最远邻
        while (!C.empty())                           // 遍历每一候选元素，包括加入的新元素
        {
            c = C.front();                              // 取出堆顶元素（最近邻）
            std::pop_heap(C.begin(), C.end(), greater); // 并弹出
            C.pop_back();
            f = W.front();                                               // 取出堆顶元素（最远邻）
            if (l2distance(c->vec, q, 128 > l2distance(f->vec, q, 128))) // 如果C中的最近邻比W中的最远邻还远，结束搜索
                break;                                                   // 弹出堆顶元素
            for (node *e : c->next[lc])                                  // 遍历c的邻接节点
            {
                if (visited.count(e) == 0) // 如果e没有被访问过
                {
                    visited[e] = true;                                                            // 标记e为访问过
                    f = W.front();                                                                // 取出W中的最远邻
                    if (W.size() < ef || l2distance(e->vec, q, 128) < l2distance(f->vec, q, 128)) // 如果W中的元素不足ef个或者e比W中的最远邻更近
                    {
                        C.push_back(e);                              // 将e加入C
                        std::push_heap(C.begin(), C.end(), greater); // 将C建立成小根堆
                        W.push_back(e);                              // 将e加入W
                        std::push_heap(W.begin(), W.end(), less);    // 将W建立成大根堆
                        if (W.size() > ef)                           // 如果W中的元素超过ef个，弹出堆顶元素(最远的点)
                        {
                            std::pop_heap(W.begin(), W.end(), less);
                            W.pop_back();
                        }
                    }
                }
            }
        }
        return W;
    }
}