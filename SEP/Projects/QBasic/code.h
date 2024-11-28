#ifndef CODE_H
#define CODE_H
#include "statement.h"

using namespace std;

class code//这个类是实现程序代码存储的类
{
public:
    map<int,statement*> statements;//存储每一句statement的结构 采用了红黑树的map便于自动有序化
    unordered_map<string,int> varlist;//存储程序运行时各个变量的值的结构 用哈希表方便查找修改
    unordered_map<string,int> varcnt;//记录程序运行中变量被使用的次数
    unordered_map<string,int> letcnt;//由于let语句的使用次数和其声明的变量有关 所以拿出来单独存储

    code();
};

void replaceOperators(string& s);//一个预处理函数 把表达式中的"MOD"和"**"转换成单字符'%'和'^' 方便处理
void replaceback(string &s);//将字符转换回去 使存储的时候符合格式
bool isValidExpression(const string& s);//预处理 判断一个字符串是否是合法的表达式
expnode* strtoexp(string &s);//把一个字符串变成一个表达式的函数
statement* parse(string s, QLineEdit* l);//分析一个字符串的语法
bool isoperator(char c);//封装一个判断是否为操作符的函数 便于调用

#endif // CODE_H
