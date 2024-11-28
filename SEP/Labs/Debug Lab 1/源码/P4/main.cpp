#include <iostream>
#include "solution.h"

using namespace std;

int main(){
        int n, m;

        cin >> n;
        cin >> m;
        cout << Solution().tilingRectangle(n, m) << endl;

        return 0;
}