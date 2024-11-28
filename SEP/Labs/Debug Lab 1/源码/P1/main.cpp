#include <iostream>
#include <vector>
#include <array>

using namespace std;

int dfs(int curr_x, int curr_y, int end_x, int end_y, vector<vector<int>> &matrix);

int main(){
        int n, m;
        int start_x, start_y;
        int end_x, end_y;

        cin >> n >> m;
        vector<vector<int>> matrix(n, vector<int> (m));
        cin >> start_x >> start_y;
        cin >> end_x >> end_y;

        for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                        cin >> matrix[i][j];

        cout << dfs(start_x, start_y, end_x, end_y, matrix) << endl;
}