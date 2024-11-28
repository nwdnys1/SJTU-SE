#include "common.h"

using namespace Minisat;

bool lab1(int states[], int n, int button[][2], int m, bool answer[])
{
    Solver solver;
    Var variables[1000];
    int i, j;
    for (i = 0; i < m; ++i)
        variables[i] = solver.newVar();

    // your solution start from here.
    // TODO: your code here



    // your solution end here.
    // solve SAT problem
    auto sat = solver.solve();
    if (sat)
    {
        for (i = 0; i < m; ++i)
            answer[i] = (solver.modelValue(variables[i]) == l_True);
        return true;
    }
    return false;
}
