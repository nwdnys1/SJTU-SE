#include "expnode.h"

expnode::expnode(int t, string v, expnode* c1 = NULL, expnode* c2 = NULL) {
    type = t;
    if (v == "%") val = "MOD";
    else if (v == "^") val = "**";
    else val = v;
    child[0] = c1;
    child[1] = c2;
}

expnode::expnode(int t, string v) {
    type = t;
    val = v;
    child[0] = NULL;
    child[1] = NULL;
}

