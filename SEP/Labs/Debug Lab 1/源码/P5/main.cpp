#include <iostream>
#include <vector>
#include "stack.h"

using namespace std;


int main(){
        int n;
        int stack_index, num;
        Stack stack_arr[3];

        for (int i = 0; i < 3; i++){
                stack_arr[i] = Stack(20);
        }

        cin >> n;

        for (int i = 0; i < n; i++){
                cin >> stack_index >> num;

                if (stack_index < 0 || stack_index >= 3){
                        cout << "invalid stack index" << endl;
                        continue;
                }

                stack_arr[stack_index].push(num);
        }

        for (int i = 0; i < 3; i++){
                cout << "stack " << i << ": ";
                while (!stack_arr[i].empty()){
                        cout << stack_arr[i].pop() << " ";
                }
                cout << endl;
        }
}