#include <iostream>
#include "solution.h"

using namespace std;

int main(){
        vector<vector<char>> board(9, vector<char> (9));
        int temp;

        for (int i = 0; i < 9; i++)
                for (int j = 0; j < 9; j++){
                        cin >> temp;
                        board[i][j] = temp == 0 ? '.' : '0' + temp;
                }

        Solution().solveSudoku(board);

        for (int i = 0; i < 9; i++){
                for (int j = 0; j < 9; j++)
                        cout << board[i][j] << " ";
                cout << endl;
        }
}