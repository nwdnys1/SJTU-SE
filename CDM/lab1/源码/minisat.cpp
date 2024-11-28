#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <zlib.h>
#include <iostream>
#include "minisat/utils/System.h"
#include "minisat/utils/ParseUtils.h"
#include "minisat/utils/Options.h"
#include "minisat/core/Dimacs.h"
#include "minisat/core/Solver.h"

using namespace Minisat;

int main()
{
    // Create a solver
    Solver solver;

    // Create variables
    auto A = solver.newVar();
    auto B = solver.newVar();
    auto C = solver.newVar();

    // Add the clauses
    // (~A v ~B v C)
    solver.addClause(~mkLit(A), ~mkLit(B), mkLit(C));
    // (~A v ~B v ~C)
    solver.addClause(~mkLit(A), ~mkLit(B), ~mkLit(C));
    // (A v ~B v C)
    solver.addClause(mkLit(A), ~mkLit(B), mkLit(C));

    // Solve the problem
    auto sat = solver.solve();

    // Check solution and retrieve model if found
    if (sat)
    {
        std::clog << "SAT\n"
                  << "Model found:\n";
        std::clog << "A := " << (solver.modelValue(A) == l_True) << '\n';
        std::clog << "B := " << (solver.modelValue(B) == l_True) << '\n';
        std::clog << "C := " << (solver.modelValue(C) == l_True) << '\n';
        return 0;
    }
    else
    {
        std::clog << "UNSAT\n";
        return 1;
    }
}
