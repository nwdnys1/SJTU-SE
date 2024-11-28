#ifndef STATEMENT_H
#define STATEMENT_H
#include "expnode.h"

class statement//这个类是用于实现语句的类
{
public:
    int type;//语句类型 0:rem, 1:let, 2:print, 3:input, 4:goto, 5:if, 6:end, 7:delete
    int order;//语句的行号
    vector<expnode*> exp;//语句所包含的表达式
    string line;//语句的内容 用于输出
    int usecnt[2]={0};//语句的执行次数 用于运行时统计
    QString op="";//语句的操作符 IF操作需要用到
    statement(int t, int o, string l);//构造函数

};

#endif // STATEMENT_H
