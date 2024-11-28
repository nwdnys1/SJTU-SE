#include <iostream>
#include <vector>
#include <algorithm>
#include "solution.h"

using namespace std;

int Solution::dfs(vector<vector<char>> &board, int index,
                vector<vector<int>> &row_avail,
                vector<vector<int>> &col_avail,
                vector<vector<int>> &grid_avail){
    if (index == to_be_solved.size()){
        return 1;
    }

    // 我们尝试填入第index个格子, 该格子在第x行, 第y列
    int x = to_be_solved[index][0];
    int y = to_be_solved[index][1];
    int ret;


    for (int k = 1; k <= 9; k++){
        if (!row_avail[y][k - 1] && !col_avail[x][k - 1] &&
            !grid_avail[to_grid_index(x, y)][k - 1]){
            row_avail[y][k - 1] = 1;
            col_avail[x][k - 1] = 1;
            grid_avail[to_grid_index(x, y)][k - 1] = 1;

            board[x][y] = '0' + k;

            ret = dfs(board, index + 1, row_avail, col_avail, grid_avail);
            if (ret)
                return ret;

            board[x][y] = '0';
        }
    }

    return 0;
}

void Solution::solveSudoku(vector<vector<char>>& board) {
    // 如果row_avail[i][k] == 0, 表示第i行可以填入数字k + 1
    vector<vector<int>> row_avail(9, vector<int> (9));
    vector<vector<int>> col_avail(9, vector<int> (9));
    vector<vector<int>> grid_avail(9, vector<int> (9));

    for (int i = 0; i < 9; i ++){
        for (int j = 0; j < 9; j++){
            if (board[i][j] != '.'){
                row_avail[i][board[i][j] - '1'] = 1;
                col_avail[j][board[i][j] - '1'] = 1;
                grid_avail[to_grid_index(i, j)][board[i][j] - '1'] = 1;
            }
        }
    }

    for (int i = 0; i < 9; i ++){
        for (int j = 0; j < 9; j++){
            int avail_count = 0;

            if (board[i][j] != '.')
                continue;

            // 计算第i行, 第j列的格子可以填入的数字个数
            for (int k = 1; k <= 9; k++){
                if (!row_avail[i][k - 1] && !col_avail[j][k - 1] &&
                        !grid_avail[to_grid_index(i, j)][k - 1]){
                    avail_count++;
                }
            }

            to_be_solved.emplace_back(vector<int> {i, j, avail_count});
        }
    }

    // 按照可填入数字的个数排序，先处理可填入数字个数少的格子
    sort(to_be_solved.begin(), to_be_solved.end(),
        [](vector<int> &left, vector<int> &right){return left[2] < right[2];}
    );

    dfs(board, 0, row_avail, col_avail, grid_avail);
}