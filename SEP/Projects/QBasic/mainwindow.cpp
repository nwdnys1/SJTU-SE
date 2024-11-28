#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "expnode.h"
#include "statement.h"

using namespace std;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    c = new code;
    ui->CodeBrowser->setFontPointSize(fontsize);//字体大小
    ui->PrintBrowser->setFontPointSize(fontsize);
    ui->TreeBrowser->setFontPointSize(fontsize);
    connect(ui->btnClear, &QPushButton::clicked, [=] {//clear操作
        if (inputing) return;
        delete c;
        c = new code;
        ui->CodeBrowser->clear();
        ui->PrintBrowser->clear();
        ui->TreeBrowser->clear();
        ui->ErrorLine->setText("清空成功！");
        });
    connect(ui->btnRun, &QPushButton::clicked, [=] {//run操作
        if (inputing) return;
        run();
        });
    connect(ui->btnLoad, &QPushButton::clicked, [=] {//load操作
        if (inputing) return;
        load();
        });


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {//处理回车
    if (event->key() == Qt::Key_Return) {
        if (inputing) {//正在输入一个变量
            QString s = ui->InputLine->text();
            ui->InputLine->setText("");
            s.remove('?');
            in = s.toStdString();
            inputing = false;
        }
        else on_cmdLineEdit_editingFinished();//输入语句或是命令
    }

}

void MainWindow::codedisplay() {//显示代码
    ui->CodeBrowser->setFontPointSize(fontsize);
    string text = "";
    for (auto it = c->statements.begin(); it != c->statements.end(); ++it) {
        text += to_string(it->second->order) + " " + it->second->line + "\n";
    }
    ui->CodeBrowser->setText(QString::fromStdString(text));
}

QString tab(int k) {//封装了一个缩进四格的函数 方便处理语法树
    QString s = "";
    for (int i = 0; i < k; ++i) s += "    ";
    return s;
}

void exptosyntax(expnode* root, int r, QString& s) {//表达式转换成语法树 用的是队列层次遍历
    queue<expnode*> q;
    queue<int> l;
    q.push(root);
    l.push(r);
    while (!q.empty()) {
        expnode* node = q.front();
        q.pop();
        int lvl = l.front();
        l.pop();
        s += tab(lvl) + QString::fromStdString(node->val) + "\n";
        if (node->child[0]) {
            q.push(node->child[0]);
            l.push(lvl + 1);
        }
        if (node->child[1]) {
            q.push(node->child[1]);
            l.push(lvl + 1);
        }
    }
}

bool MainWindow::input(string s) {//输入变量操作
    try {

        ui->InputLine->setText("?");
        inputing = true;
        while (inputing) {//网上查阅了这个函数 会让其他线程执行一部分时间 可以使单线程不被循环阻塞 input操作时就不会卡死 当然用多线程应该也可以实现
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        for (int k = 0; k < in.size(); k++) if (!isdigit(in[k]) && !(in[k] == '-' && k == 0)) throw QString("请输入一个合法的数值！");
        c->varlist[s] = stoi(in);
    }
    catch (QString& s) {
        ui->ErrorLine->setText(s);
        return false;
    }
    catch (std::out_of_range&) {
        ui->ErrorLine->setText("输入的数值太大！");
        return false;
    }
    return true;
}

void MainWindow::treedisplay() {//展示语法树
    ui->TreeBrowser->clear();
    ui->TreeBrowser->setFontPointSize(fontsize);
    QString s;
    for (auto it = c->statements.begin(); it != c->statements.end(); ++it) {
        statement* tmp = it->second;
        switch (tmp->type) {
        case 0: {
            string s = to_string(it->second->order) + " " + tmp->line;
            int i = s.find("REM");
            i += 4;
            s.insert(i, "\n    ");
            s += "\n";
            ui->TreeBrowser->append(QString::fromStdString(s));
            break;
        }
        case 1: {
            s = s.number(tmp->order) + " " + "LET = " + s.number(c->letcnt[tmp->exp[0]->val]) + "\n";
            s += tab(1) + QString::fromStdString(tmp->exp[0]->val) + " " + s.number(c->varcnt[tmp->exp[0]->val]) + "\n";
            exptosyntax(tmp->exp[1], 1, s);
            ui->TreeBrowser->append(s);
            break;
        }
        case 2: {
            s = s.number(tmp->order) + " " + "PRINT " + s.number(tmp->usecnt[0]) + "\n";
            exptosyntax(tmp->exp[0], 1, s);
            ui->TreeBrowser->append(s);
            break;
        }
        case 3: {
            s = s.number(tmp->order) + " " + "INPUT " + s.number(tmp->usecnt[0]) + "\n";
            s += tab(1) + QString::fromStdString(tmp->exp[0]->val) + " " + s.number(c->varcnt[tmp->exp[0]->val]) + "\n";
            ui->TreeBrowser->append(s);
            break;
        }
        case 4: {
            s = s.number(tmp->order) + " " + "GOTO " + s.number(tmp->usecnt[0]) + "\n";
            s += tab(1) + QString::fromStdString(tmp->exp[0]->val) + "\n";
            ui->TreeBrowser->append(s);
            break;
        }
        case 5: {
            s = s.number(tmp->order) + " " + "IF THEN " + s.number(tmp->usecnt[0]) + " " + s.number(tmp->usecnt[1]) + "\n";
            exptosyntax(tmp->exp[0], 1, s);
            s += tab(1) + tmp->op + "\n";
            exptosyntax(tmp->exp[1], 1, s);
            exptosyntax(tmp->exp[2], 1, s);
            ui->TreeBrowser->append(s);
            break;
        }
        case 6: {
            s = s.number(tmp->order) + " " + "END " + "\n";
            ui->TreeBrowser->append(s);
            break;
        }

        }
    }
}

void MainWindow::on_cmdLineEdit_editingFinished()//输入语句或者命令
{
    try {

        QString cmd = ui->InputLine->text();
        if (cmd == "") return;//忽略空字符串
        if (cmd == "QUIT") this->close();
        if (cmd == "HELP") {//显示帮助
            QImage* img = new QImage;
            bool ld = img->load("E:/oneDrive - sjtu.edu.cn/COLLEGE/SEP/QBasic/QBasic/help.png");
            QLabel* h = new QLabel;
            h->resize(img->width(), img->height());
            h->setPixmap(QPixmap::fromImage(*img));
            h->show();
            ui->InputLine->setText("");
            return;
        }
        ui->InputLine->setText("");
        directlyexec(cmd.toStdString());//先处理不带行号的指令
        statement* s = parse(cmd.toStdString(), ui->ErrorLine);
        if (!s) return;//存在错误或是一条直接执行的指令
        if (s->type == 7) {//删除
            if (c->statements.count(s->order)) c->statements.erase(s->order);
            else throw QString("无效的操作：无法删除不存在的语句");
        }
        else {//修改或新增语句
            if (c->statements.count(s->order)) delete c->statements[s->order];
            c->statements[s->order] = s;
        }
        ui->ErrorLine->setText("操作成功！");
        codedisplay();
    }
    catch (QString s) {
        ui->ErrorLine->setText(s);
        return;
    }
}

void MainWindow::directlyexec(string s) {//不带行号的语句
    ui->PrintBrowser->setFontPointSize(fontsize);
    int i = 0;
    while (s[i] == ' ') {//跳过空格
        i++;
    }
    int j = s.find(" ", i);
    if (j == string::npos) j = s.size();
    string t = s.substr(i, j - i);
    i = j;
    while (s[i] == ' ') {
        i++;
    }
    if (t == "LET") {
        if (i == s.size()) throw QString("不合法的语句：LET语句缺少表达式");
        if ((j = s.find("=", i)) == string::npos) throw QString("错误：LET语句没有“=”");
        string svar = s.substr(i, j - i), sval = s.substr(j + 1);
        expnode* var = strtoexp(svar);
        expnode* val = strtoexp(sval);
        if (var->type != 1) throw QString("不合法的语句：LET语句的左值必须是一个变量");
        if (!c->varlist.count(svar)) c->varlist[svar] = 0;
        c->varlist[svar] = calculate(val, INT_MIN);
        ui->PrintBrowser->append(QString::fromStdString("LET " + svar + " = " + sval + ": ") + QString::number(calculate(val, INT_MIN)));
        ui->PrintBrowser->moveCursor(QTextCursor::End);
        throw QString("操作成功！");
    }
    if (t == "PRINT") {
        if (i == s.size()) throw QString("不合法的语句：PRINT语句缺少表达式");
        string sval = s.substr(i);
        expnode* val = strtoexp(sval);
        ui->PrintBrowser->append("PRINT " + QString::fromStdString(sval) + ": " + QString::number(calculate(val, INT_MIN)));
        ui->PrintBrowser->moveCursor(QTextCursor::End);
        throw QString("操作成功！");
    }
    if (t == "INPUT") {
        if (i == s.size()) throw QString("不合法的语句：INPUT语句缺少变量名");
        string svar = s.substr(i);
        if (strtoexp(svar)->type != 1) throw QString("不合法的语句：INPUT语句的表达式必须是一个变量");
        if (!c->varlist.count(svar)) c->varlist[svar] = 0;
        while (!input(svar)) {}
        ui->PrintBrowser->append(QString::fromStdString("INPUT " + svar + ": ? " + to_string(c->varlist[svar])));
        ui->PrintBrowser->moveCursor(QTextCursor::End);
        throw QString("操作成功！");
    }

}

void MainWindow::run() {//运行程序 基本思路就是用c++去模拟运行
    try {

        ui->PrintBrowser->setFontPointSize(fontsize);
        if (c->statements.empty()) return;//代码为空
        ui->PrintBrowser->setText("");
        ui->TreeBrowser->setText("");
        for (auto i = c->statements.begin(); i != c->statements.end(); ++i) {//清空语句的运行记录数
            i->second->usecnt[0] = 0;
            i->second->usecnt[1] = 0;
        }
        c->varlist.clear();
        c->varcnt.clear();
        c->letcnt.clear();
        QString output = "";
        //运行代码
        auto it = c->statements.begin();
        while (it != c->statements.end()) {
            statement* tmp = it->second;
            tmp->usecnt[0] += 1;
            switch (tmp->type) {
            case 0: {
                break;
            }
            case 1: {
                if (!c->varlist.count(tmp->exp[0]->val)) {
                    c->letcnt[tmp->exp[0]->val] = 0;
                    c->varlist[tmp->exp[0]->val] = 0;
                    c->varcnt[tmp->exp[0]->val] = 0;
                }
                c->letcnt[tmp->exp[0]->val] += 1;
                c->varcnt[tmp->exp[0]->val] += 1;
                c->varlist[tmp->exp[0]->val] = calculate(tmp->exp[1], tmp->order);
                break;
            }
            case 2: {
                output += QString::fromStdString(tmp->line) + ": " + QString::number(calculate(tmp->exp[0], tmp->order)) + "\n";
                break;
            }
            case 3: {
                if (!c->varlist.count(tmp->exp[0]->val)) {
                    c->varlist[tmp->exp[0]->val] = 0;
                    c->varcnt[tmp->exp[0]->val] = 0;
                }
                c->varcnt[tmp->exp[0]->val] += 1;
                while (!input(tmp->exp[0]->val)) {}
                output += QString::fromStdString(tmp->line + ": ? " + to_string(c->varlist[tmp->exp[0]->val]) + "\n");
                break;
            }
            case 4: {
                int o = stoi(tmp->exp[0]->val);
                if (!c->statements.count(o)) throw QString("运行中存在错误：语句" + QString::number(tmp->order) + "GOTO跳转的行号不存在");
                it = c->statements.find(o);
                continue;
                break;
            }
            case 5: {
                int o = stoi(tmp->exp[2]->val), l = calculate(tmp->exp[0], tmp->order), r = calculate(tmp->exp[1], tmp->order);
                bool flag;
                if (tmp->op == "=") flag = l == r;
                else if (tmp->op == ">") flag = l > r;
                else if (tmp->op == "<") flag = l < r;
                if (!c->statements.count(o)) throw QString("运行中存在错误：语句" + QString::number(tmp->order) + "THEN跳转的行号不存在");
                if (flag) {
                    it = c->statements.find(o);
                    continue;
                }
                else {
                    tmp->usecnt[0] -= 1;
                    tmp->usecnt[1] += 1;
                }
                break;
            }
            case 6: {
                goto END;
            }
            }
            it++;
        }
    END:
        ui->PrintBrowser->setText(output);
        ui->PrintBrowser->moveCursor(QTextCursor::End);
        ui->ErrorLine->setText("运行成功！");
        treedisplay();
    }
    catch (QString s) {
        ui->ErrorLine->setText(s);
        return;
    }
}

void MainWindow::load() {//加载代码 等价于一行一行的读取
    try {

        QFileDialog d;
        QString filename = d.getOpenFileName(this, "加载代码", "C:/Users/Administrator/Desktop", "txt文件(*.txt)");
        if (filename == "") return;
        QFile file(filename);
        code* ld = new code;
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            QString line;
            while (!in.atEnd()) {
                line = in.readLine();
                if (line == "") continue;//
                if (line == "QUIT") this->close();
                if (line == "HELP") {
                    QImage* img = new QImage;
                    bool ld = img->load(":/new/prefix1/help1.png");
                    QLabel* h = new QLabel;
                    h->resize(img->width(), img->height());
                    h->setPixmap(QPixmap::fromImage(*img));
                    h->show();
                    continue;
                }
                if (line == "RUN") {
                    c = ld;
                    ui->ErrorLine->setText("输入成功！");
                    codedisplay();
                    run();
                    continue;
                }
                if (line == "CLEAR") {
                    ui->btnClear->click();
                    continue;
                }
                directlyexec(line.toStdString());
                statement* s = parse(line.toStdString(), ui->ErrorLine);
                if (!s) return;//存在错误
                if (s->type == 7) {//删除
                    if (ld->statements.count(s->order)) ld->statements.erase(s->order);
                    else throw QString("无效的操作：无法删除不存在的语句");
                }
                else {//修改或新增语句
                    if (ld->statements.count(s->order)) delete ld->statements[s->order];
                    ld->statements[s->order] = s;
                }
            }
            file.close();
        }
        c = ld;
        ui->ErrorLine->setText("输入成功！");
        codedisplay();
    }
    catch (QString s) {
        ui->ErrorLine->setText(s);
        return;
    }
}

int MainWindow::calculate(expnode* root, int o) {//计算表达式树的值 递归实现
    if (root->type == 1) {
        if (!c->varlist.count(root->val)) {
            if (o == INT_MIN) throw QString("错误：变量" + QString::fromStdString(root->val) + "未被创建就被使用");
            else throw QString("运行中存在错误：语句" + QString::number(o) + "的变量" + QString::fromStdString(root->val) + "未被创建就被使用");
        }
        c->varcnt[root->val] += 1;
        return c->varlist[root->val];
    }
    if (root->type == 2) return stoi(root->val);
    if (root->val == "+") return calculate(root->child[0], o) + calculate(root->child[1], o);
    if (root->val == "-") return calculate(root->child[0], o) - calculate(root->child[1], o);
    if (root->val == "*") return calculate(root->child[0], o) * calculate(root->child[1], o);
    if (root->val == "/") {
        int r = calculate(root->child[1], o);
        if (r == 0) {
            if (o == INT_MIN) throw QString("错误：尝试除0");
            else throw QString("运行中存在错误：语句" + QString::number(o) + "尝试除0");
        }
        return calculate(root->child[0], o) / r;
    }
    if (root->val == "MOD") {
        int l = calculate(root->child[0], o), r = calculate(root->child[1], o);
        if (r == 0) {
            if (o == INT_MIN) throw QString("错误：尝试用0取模");
            else throw QString("运行中存在错误：语句" + QString::number(o) + "尝试用0取模");
        }
        return r * l >= 0 ? l % r : l - (l - r - (r < 0 ? 1 : -1)) / r * r;
    }
    if (root->val == "**") return pow(calculate(root->child[0], o), calculate(root->child[1], o));
    return 0;
}
