#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTextBrowser>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QBrush>
#include <QPushButton>
#include <QPen>
#include <QFileDialog>
#include<code.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow//这个类是主界面类
{
    Q_OBJECT

public:
    code* c;//这个应用的程序代码
    int fontsize=16;//字体大小
    bool inputing=false;//一个布尔变量用于输入操作
    string in="";//暂存输入变量的字符串

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;//处理回车事件
    void run();//运行代码
    void load();//载入代码
    void directlyexec(string s);//处理直接执行的语句(LET,INPUT,PRINT)
    int calculate(expnode* root,int order);//计算一棵表达式树的值的函数
    void treedisplay();//展示表达式树
    void codedisplay();//展示存储的代码
    bool input(string val);//用于处理INPUT操作的函数

private slots:
    void on_cmdLineEdit_editingFinished();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
