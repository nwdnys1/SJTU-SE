#include <vector>

class Solution {
    void dfs(std::vector<std::vector<int>> &matrix, int remain, int used, int n, int m, int &best);

public:
    int tilingRectangle(int n, int m);
};