#include "statement.h"

statement::statement(int t, int o = 1, string l = "") {
    exp.clear();
    type = t;
    order = o;
    line = l;
}
