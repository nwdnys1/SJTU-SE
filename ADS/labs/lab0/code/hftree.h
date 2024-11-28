#ifndef HFTREE_H
#define HFTREE_H

#include <string>
#include <map>
#include <queue>
#include <algorithm>

class hfTree
{

private:
    // TODO: Your code here
    struct Node
    {
        std::string data;//order of the characters 
        int freq;
        Node *left;
        Node *right;
        Node(std::string data, int freq, Node *left, Node *right) : data(data), freq(freq), left(left), right(right) {}
    };
    struct compare
    {
        bool operator()(Node *l, Node *r)
        {   if(l->freq==r->freq) return (l->data > r->data);
            return (l->freq > r->freq);
        }
    };
    Node *root;

public:
    enum class Option
    {
        SingleChar,
        MultiChar
    };
    hfTree(const std::string &text, const Option op);
    Node* buildSingleCharHfTree (const std::string &text);
    Node* buildMultiCharHfTree (const std::string &text);
    std::map<std::string, std::string> getCodingTable();
    std::map<std::string, std::string> dfs(Node *root, std::string code);
};

#endif