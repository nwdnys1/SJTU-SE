#include "code.h"

code::code() {

}

bool isoperator(char c) {//封装一个判断操作符的函数
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

void replaceOperators(string& expression) {//替换操作符MOD和**
    size_t found = expression.find("**");
    while (found != string::npos) {
        expression.replace(found, 2, "^");
        found = expression.find("**", found + 1);
    }
    found = expression.find("MOD");
    while (found != string::npos) {
        expression.replace(found, 3, "%");
        found = expression.find("MOD", found + 1);
    }
}

void replaceback(string& expression) {//替换回原来
    size_t found = expression.find("^");
    while (found != string::npos) {
        expression.replace(found, 1, "**");
        found = expression.find("^", found + 1);
    }
    found = expression.find("%");
    while (found != string::npos) {
        expression.replace(found, 1, "MOD");
        found = expression.find("%", found + 1);
    }
}

bool isValidExpression(string& s) {//判断表达式是否合法 主要通过1.运算数和操作符数量是否匹配2.括号的位置是否合法3.括号数量是否匹配 来判断
    int i = 0, opcnt = 0, numcnt = 0, brcnt = 0;//分别记录操作符数量 运算数数量 和括号数量（左-右）
    while (i < s.length()) {
        if (isspace(s[i])) {
            s.erase(i, 1);
            continue;
        }
        if (s[i] == '-' && i < s.length() - 1) {//遇到一个负号
            int j = i + 1;
            while (s[j] == ' ') j++;
            if ((isoperator(s[i - 1]) || s[i - 1] == '(' || i == 0) && isdigit(s[j])) {
                i++;
                continue;
            }
        }
        if (isalpha(s[i])) {
            if (numcnt++ != opcnt) throw QString("不合法的表达式：运算符与操作数不匹配");
            while (i < s.length() && (isalpha(s[i]) || isdigit(s[i]))) i++;
        }
        else if (isdigit(s[i])) {
            if (numcnt++ != opcnt) throw QString("不合法的表达式：运算符与操作数不匹配");
            while (i < s.length() && isdigit(s[i])) i++;
        }
        else if (isoperator(s[i])) {
            if (++opcnt != numcnt) throw QString("不合法的表达式：运算符与操作数不匹配");
            i++;
        }
        else if (s[i] == '(') {
            int j = i + 1;
            while (s[j] == ' ') j++;
            if (j == s.length() || (isoperator(s[j]) && s[j] != '-')) throw QString("不合法的表达式：左括号的右侧必须为操作数或左括号");
            if (i != 0 && (isdigit(s[i - 1]) || isalpha(s[i - 1]))) throw QString("不合法的表达式：左括号的左侧必须为运算符或左括号");
            brcnt++;
            i++;
        }
        else if (s[i] == ')') {
            int j = i + 1;
            while (s[j] == ' ') j++;
            if (j != s.length() && (isdigit(s[j]) || isalpha(s[j]))) throw QString("不合法的表达式：右括号的右侧必须为运算符或右括号");
            if (i == 0 || isoperator(s[i - 1])) throw QString("不合法的表达式：右括号的左侧必须为操作数或右括号");
            brcnt--;
            i++;
        }
        else throw QString("不合法的表达式：字符‘" + QString(s[i]) + "’不合法");
    }
    if (brcnt) throw QString("不合法的表达式：括号数量不匹配");
    if (s == "") throw QString("不合法的表达式：表达式不能为空");
    return true;
}

expnode* strtoexp(string& s) {//把字符串转变为表达式树 基本思路是利用栈结构 入栈的如果是运算符 要把栈里所有优先级比当前运算符高的运算符都拿出来进行建树
    unordered_map<char, int> pri = { {'+',1},{'-',1},{'*',2},{'/',2},{'%',2},{'^',3},{'(',0},{')',0} };//哈希一个优先级
    replaceOperators(s);//替换为单字符方便处理
    if (!isValidExpression(s)) {//如果表达式不合法 返回空
        return NULL;
    }
    stack<char> opStack;//运算符栈
    stack<expnode*> nodeStack;//操作数栈
    int i = 0;
    int len = s.length();
    while (i < len) {
        if (s[i] == '-') {
            if ((isoperator(s[i - 1]) || s[i - 1] == '(' || i == 0) && isdigit(s[i + 1])) {//遇到'-' 判断其为负号 需要在负号前添加一个0
                int j = i + 1;
                while (isdigit(s[j])) {
                    j++;
                }
                expnode* num = new expnode(2, s.substr(i + 1, j - i - 1));
                expnode* zero = new expnode(2, "0");
                nodeStack.push(new expnode(0, "-", zero, num));
                i = j;
            }
        }
        if (isalpha(s[i])) {//变量
            string var = "";
            while (i < len && (isalpha(s[i]) || isdigit(s[i]))) {
                var += s[i];
                i++;
            }
            nodeStack.push(new expnode(1, var));
        }
        else if (isdigit(s[i])) {//常量
            string num = "";
            while (i < len && isdigit(s[i])) {
                num += s[i];
                i++;
            }
            nodeStack.push(new expnode(2, num));
        }
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '%') {//运算符
            while (!opStack.empty() && pri[opStack.top()] >= pri[s[i]]) {
                string op = "";
                op += opStack.top();
                opStack.pop();
                expnode* right = nodeStack.top();
                nodeStack.pop();
                expnode* left = nodeStack.top();
                nodeStack.pop();
                nodeStack.push(new expnode(0, op, left, right));
            }
            opStack.push(s[i]);
            i++;
        }
        else if (s[i] == '^') {//**运算因为是右结合需要拿出来特判 一个“**”运算符进栈时 没有运算符可能比它优先级高（包括**） 所以只需要进栈
            opStack.push(s[i]);
            i++;
        }
        else if (s[i] == '(') {//左括号入栈
            opStack.push(s[i]);
            i++;
        }
        else if (s[i] == ')') {//右括号 需要把括号内的所有运算符都进行计算 直至遇到对应的左括号
            while (opStack.top() != '(') {
                string op = "";
                op += opStack.top();
                opStack.pop();
                expnode* right = nodeStack.top();
                nodeStack.pop();
                expnode* left = nodeStack.top();
                nodeStack.pop();
                nodeStack.push(new expnode(0, op, left, right));
            }
            opStack.pop();
            i++;
        }

    }
    while (!opStack.empty()) {//处理栈里剩下的操作数和运算符
        string op = "";
        op += opStack.top();
        opStack.pop();
        expnode* right = nodeStack.top();
        nodeStack.pop();
        expnode* left = nodeStack.top();
        nodeStack.pop();
        nodeStack.push(new expnode(0, op, left, right));
    }
    replaceback(s);
    return nodeStack.top();
}

statement* parse(string s, QLineEdit* l) {//分析语句
    try {

        unordered_map<string, int> type = { {"REM", 0}, {"LET", 1}, {"PRINT", 2}, {"INPUT", 3}, {"GOTO", 4}, {"IF", 5}, {"END", 6} };
        string order = "";
        int i = 0;
        //处理order：
        while (s[i] == ' ') {//跳过空格
            i++;
        }
        int j = s.find(" ", i);
        for (int k = i; k < j; k++) if (!isdigit(s[k]) && !(s[k] == '-' && k == 0)) throw QString("不合法的语句：语句的行号不正确");
        int o = stoi(s.substr(i, j - i));
        //处理type：
        if (j == string::npos) return new statement(7, o, "");//delete
        i = j + 1;
        while (s[i] == ' ') {//跳过空格
            i++;
        }
        if (i == s.size()) return new statement(7, o, "");//delete
        j = s.find(" ", i);
        if (j == string::npos) j = s.size();
        if (!type.count(s.substr(i, j - i))) throw QString("不合法的语句：语句的类型不正确");
        int t = type[s.substr(i, j - i)];
        //处理语句：（基本思路就是找到对应的字符进行分析 比如LET语句中的比较符和THEN 其中就是两个表达式和一个行号
        statement* stt = new statement(t, o, "");
        i = j;
        while (s[i] == ' ') {
            i++;
        }
        switch (t) {
        case 0: {
            if (i == s.size()) throw QString("不合法的语句：REM语句后不能为空");
            stt->line = "REM " + s.substr(i);
            break;
        }
        case 1: {
            if (i == s.size()) throw QString("不合法的语句：LET语句缺少表达式");
            if ((j = s.find("=", i)) == string::npos) throw QString("错误：LET语句没有“=”");
            string svar = s.substr(i, j - i), sval = s.substr(j + 1);
            expnode* var = strtoexp(svar);
            if (var->type != 1) throw QString("不合法的语句：LET语句的左值必须是一个变量");
            stt->exp.push_back(var);
            expnode* val = strtoexp(sval);
            stt->exp.push_back(val);
            stt->line = "LET " + svar + " = " + sval;
            break;
        }
        case 2: {
            if (i == s.size()) throw QString("不合法的语句：PRINT语句缺少表达式");
            string sval = s.substr(i);
            expnode* val = strtoexp(sval);
            stt->exp.push_back(val);
            stt->line = "PRINT " + sval;
            break;
        }
        case 3: {
            if (i == s.size()) throw QString("不合法的语句：INPUT语句缺少变量名");
            string svar = s.substr(i);
            expnode* var = strtoexp(svar);
            if (var->type != 1) throw QString("不合法的语句：INPUT语句的表达式必须是一个变量");
            stt->exp.push_back(var);
            stt->line = "INPUT " + svar;
            break;
        }
        case 4: {
            if (i == s.size()) throw QString("不合法的语句：GOTO语句缺少跳转行数");
            string sval = s.substr(i);
            expnode* val = strtoexp(sval);
            stt->exp.push_back(val);
            stt->line = "GOTO " + sval;
            break;
        }
        case 5: {
            if (i == s.size()) throw QString("不合法的语句：IF语句缺少表达式");
            int thenpos;
            if ((thenpos = s.find("THEN", i)) == string::npos) throw QString("不合法的语句：IF语句没有“THEN”");
            if ((j = s.find_first_of(">=<", i)) == string::npos) throw QString("不合法的语句：IF语句没有比较运算符“> = <”");
            stt->op = s[j];
            string sl = s.substr(i, j - i), sr = s.substr(j + 1, thenpos - j - 1), sval = s.substr(thenpos + 4);
            expnode* left = strtoexp(sl);
            stt->exp.push_back(left);
            expnode* right = strtoexp(sr);
            stt->exp.push_back(right);
            expnode* val = strtoexp(sval);
            stt->exp.push_back(val);
            stt->line = "IF " + sl + " " + stt->op.toStdString() + " " + sr + " THEN " + sval;
            break;
        }
        case 6: {
            if (i != s.size()) throw QString("不合法的语句：END语句不符合格式");
            stt->line = "END";
            break;
        }
        }

        return stt;

    }
    catch (std::invalid_argument&) {
        l->setText("不合法的语句：语句缺少正确的行号");
        return NULL;
    }
    catch (std::out_of_range&) {
        l->setText("不合法的语句：语句的行号太大");
        return NULL;
    }
    catch (QString s) {
        l->setText(s);
        return NULL;
    }
}
