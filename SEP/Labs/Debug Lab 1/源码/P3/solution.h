#include <vector>

using namespace std;

class Solution {
    vector<vector<int>> to_be_solved;

    static inline int to_grid_index(int i, int j){
        return (i / 3) * 3 + j / 3;
    }

    int dfs(vector<vector<char>> &board, int index,
                    vector<vector<int>> &row_avail,
                    vector<vector<int>> &col_avail,
                    vector<vector<int>> &grid_avail);

public:
    void solveSudoku(vector<vector<char>>& board);
};