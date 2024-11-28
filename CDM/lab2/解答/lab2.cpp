#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include"z3++.h"
using std::string;
using std::vector;
using std::map;
using std::stack;
using namespace z3;

#include "multree.h"
#include "cfg.h"
#include "minisee.h"

#include<iostream>
//下面两个print函数或许能帮你debug
//输出程序流图root对应的代码
void print_cfg(cfg_node* root);
//输出表达式树root存储的表达式
void print_exp(exp_node* root);

extern stack<see_state> state_queue;


//把字符串表示的数字转成int类型带符号整数
//s可能是10进制无符号整数、10进制带符号整数或16进制整数
int mystoi(string s) {
    long long val = strtol(s.c_str(), NULL, 0);
    return (int)val;
	
}


//root是一棵表达式树
//如果root为NULL，就返回NULL
//否则，就生成一棵新的树，这棵树表示的表达式和root表示的完全一致
//但是这棵树的所有节点都是new出来的，不可以复用root中的任何节点
exp_node* copy_exp_tree(exp_node* root) {
    if(root == NULL) {
        return NULL;
    }
    exp_node* new_root = new exp_node(root->type, root->val,NULL);
    stack<exp_node*> s1, s2;
    exp_node* old= root;
    exp_node* cur = new_root;
    s1.push(new_root);
    s2.push(root);
    while(!s1.empty()) {
        old = s2.top();
        cur = s1.top();
        s1.pop();
        s2.pop();
        if(old->child[0]) {
            cur->child[0] = new exp_node(old->child[0]->type, old->child[0]->val,NULL);
            s1.push(cur->child[0]);
            s2.push(old->child[0]);
        }
        if(old->child[1]) {
            cur->child[1] = new exp_node(old->child[1]->type, old->child[1]->val, NULL);
            s1.push(cur->child[1]);
            s2.push(old->child[1]);
        }
    }
    return new_root;
    
}


//这个函数用于处理if语句
//state是if语句对应的符号执行节点
//path_const是state里面的path constraint
//condition表示if条件表达式
//branch是true时返回if条件为真对应的新的path constraint
//branch是false时返回if条件为假对应的新的path constraint
exp_node* update_path_const(see_state* state, exp_node* path_const, bool branch, exp_node* condition) {
    
    //old_const是和state里面的path constraint一样的表达式
    exp_node* old_const = copy_exp_tree(path_const);
    //cur_const是当前if条件对应的表达式
    exp_node* cur_const = update_exp_tree(state, condition);

    //exp是新的path constraint
    exp_node* exp = NULL;
    if(old_const) {
        //如果当前state里面的path constraint不是true
        exp = new exp_node(exp_op, "&&", old_const);
        if(branch) {
            exp->child[1] = cur_const;
        }
        else {
            exp->child[1] = new exp_node(exp_op, "!", cur_const);
        }
    }
    else {
        if(branch) {
            exp = cur_const;
        }
        else {
            exp = new exp_node(exp_op, "!", cur_const);
        }
    }

    return exp;
}


//处理if语句
//state中的cfg第一条语句就是一个if语句
void analyze_if(see_state* state) {
    see_state* false_state = new see_state();
    false_state->sym_store=state->sym_store;
    cfg_node* root= state->cfg;
    false_state->cfg=root->next[1];
    false_state->path_const=update_path_const(state, state->path_const, false, root->exp_tree);
    state->path_const=update_path_const(state, state->path_const, true, root->exp_tree);
    state->cfg=root->next[0];
    state_queue.push(*state);
    state_queue.push(*false_state);
    //your code here, you may need to use update_path_const
    return;
}




