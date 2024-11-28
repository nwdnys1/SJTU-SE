#ifndef EXPNODE_H
#define EXPNODE_H
#include <iostream>
#include <string>
#include <QString>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <cmath>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QDebug>

using namespace std;

class expnode//这个类是实现表达式树节点的结构
{
public:
    int type;//节点类型 0:操作符 1:变量 2:常量
    string val;//节点的值
    expnode* child[2];//节点的左右儿子
    expnode(int t, string v, expnode* c1, expnode* c2);//构造函数
    expnode(int t, string v);

};

#endif // EXPNODE_H
