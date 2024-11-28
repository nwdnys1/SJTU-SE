#include "hftree.h"

hfTree::hfTree(const std::string &text, const Option op)
{
    // TODO: Your code here
    if(op==Option::SingleChar) root=buildSingleCharHfTree(text);
    else root=buildMultiCharHfTree(text);


}

hfTree::Node* hfTree::buildSingleCharHfTree (const std::string &text)
{
    std::map<char, int> frequencyMap;
    for (char c : text) {
        if(frequencyMap.find(c)!=frequencyMap.end()) frequencyMap[c]++;
        else frequencyMap[c]=1;
    }

    std::priority_queue<Node*, std::vector<Node*>, compare> pq;

    for (auto it = frequencyMap.begin(); it != frequencyMap.end(); ++it) {
        Node* newNode = new Node(std::string(1, it->first), it->second, nullptr, nullptr);
        pq.push(newNode);
    }

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        Node* merged = new Node(std::min(left->data,right->data), left->freq + right->freq, left, right);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    root = pq.top();
    return root;
}

hfTree::Node* hfTree::buildMultiCharHfTree (const std::string &text)
{
   std::map<std::string, int> frequencyMapof2;
     for (int i = 0; i < text.size(); ++i) {
        if (i < text.size() - 1) {
            frequencyMapof2[text.substr(i, 2)]++;
        }
    }
    std::vector<std::pair<std::string, int>> sortedMap(frequencyMapof2.begin(), frequencyMapof2.end());
    std::sort(sortedMap.begin(), sortedMap.end(), [](std::pair<std::string, int> a, std::pair<std::string, int> b) {
        if(a.second==b.second) return a.first < b.first;
        return a.second > b.second;
    });
    std::map<std::string, int> top3;
    for (int i = 0; i < 3; ++i) {
        top3[sortedMap[i].first] = sortedMap[i].second;
    }

    std::map<std::string, int> frequencyMap;
    for(int i=0;i<text.size();++i){
        if(i<text.size()-1){
            std::string temp=text.substr(i,2);
            if(top3.find(temp)!=top3.end()){
                frequencyMap[temp]++;
                i++;
            }
            else frequencyMap[std::string(1,text[i])]++;
        }
        else frequencyMap[std::string(1,text[i])]++;
    }


    std::priority_queue<Node*, std::vector<Node*>, compare> pq;

    for (auto it = frequencyMap.begin(); it != frequencyMap.end(); ++it) {
        Node* newNode = new Node(it->first, it->second, nullptr, nullptr);
        pq.push(newNode);
    }

    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        Node* merged = new Node(std::min(left->data,right->data), left->freq + right->freq, left, right);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    root = pq.top();
    return root;
}

std::map<std::string, std::string> hfTree::getCodingTable()
{
    // TODO: Your code here
    return dfs(root, "");
}

std::map<std::string, std::string> hfTree::dfs(Node *root, std::string code)
{
    std::map<std::string, std::string> codingTable;
    if (root == nullptr) {
        return codingTable;
    }

    if (root->left == nullptr && root->right == nullptr) {
        codingTable[root->data] = code;  // 叶子结点，保存编码
    }

    // 递归获取左子树和右子树的编码
    std::map<std::string, std::string> leftTable = dfs(root->left, code + "0");
    std::map<std::string, std::string> rightTable = dfs(root->right, code + "1");

    // 合并左右子树的编码表
    codingTable.insert(leftTable.begin(), leftTable.end());
    codingTable.insert(rightTable.begin(), rightTable.end());

    return codingTable;
}
