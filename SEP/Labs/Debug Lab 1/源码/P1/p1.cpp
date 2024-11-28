#include <vector>
#include <array>

using namespace std;

int dfs(int curr_x, int curr_y, int end_x, int end_y, vector<vector<int>> &matrix){
        int n = matrix.size();
        int m = matrix[0].size();
        array<pair<int, int>, 4> directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
        int ret = 0;

        if (curr_x == end_x && curr_y == end_y)
                return 1;

        if (matrix[curr_x][curr_y] == 1)
                return 0;

        matrix[curr_x][curr_y] = 1;

        for (int i = 0; i < 4; i++){
                if (ret = dfs(curr_x + directions[i].first, curr_y + directions[i].second, end_x, end_y, matrix))
                        return ret;
        }

        matrix[curr_x][curr_y] = 0;

        return ret;
}