#include <iostream>
#include <climits>
#include <string>
#include <utility>

#include "board.h"

using namespace std;

int main() {
    while (true) {
        cout << "How many disks do you want? (1 ~ 5)" << endl;

        string input;
        getline(cin, input);
        if (input == "Q") {
            break;
        }

        try {
            int n = stoi(input);
            if (n >= 1 && n <= 5) {
                Board board(n);
                board.draw();

                while (true) {
                    cout << "Move a disk. Format: x y" << endl;
                    int from, to;
                    cin >> from >> to;
                    cin.ignore(INT_MAX, '\n');

                    if (!cin.good()) {
                        cin.clear();
                    } else if (from == 0 && to == 0) {
                        board.autoplay();
                        cout << "Congratulations! You win!" << endl;
                        break;
                    } else {
                        board.move(from, to);
                    }

                    board.draw();
                    if (board.win()) {
                        cout << "Congratulations! You win!" << endl;
                        break;
                    }
                }
            }
        } catch (...) {}
    }

    return 0;
}
