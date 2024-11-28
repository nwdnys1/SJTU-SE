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
    int switchs[1000][2]={0};
    
    for(i=0;i<m;i++){
        if(switchs[button[i][0]-1][0]) switchs[button[i][0]-1][1]=i+1;
        else switchs[button[i][0]-1][0]=i+1;
        if(button[i][1]) {
            if(switchs[button[i][1]-1][0]) switchs[button[i][1]-1][1]=i+1;
            else switchs[button[i][1]-1][0]=i+1;
        }
    }
    
    for(i=0;i<n;i++){
        if(states[i]){
            if(switchs[i][1]){
                solver.addClause(mkLit(variables[switchs[i][0]-1]),~mkLit(variables[switchs[i][1]-1]));
                solver.addClause(~mkLit(variables[switchs[i][0]-1]),mkLit(variables[switchs[i][1]-1]));
            }
            else solver.addClause(~mkLit(variables[switchs[i][0]-1]));
        }
        else{
            if(switchs[i][1]){
                solver.addClause(~mkLit(variables[switchs[i][0]-1]),~mkLit(variables[switchs[i][1]-1]));
                solver.addClause(mkLit(variables[switchs[i][0]-1]),mkLit(variables[switchs[i][1]-1]));
            }
            else solver.addClause(mkLit(variables[switchs[i][0]-1]));
        }
          
    }
    
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
