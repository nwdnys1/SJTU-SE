#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <singleplayer.h>
#include <QTimer>
#include <QFileDialog>
#include <QDialog>
#include <QPushButton>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //这个类是游戏主界面类，下面的代码进行了ui的初始化以及按钮功能的绑定；
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    this->setFont(font);
    this->setFixedSize(w, h);
    this->setWindowTitle("连连看");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    QPushButton* btn1 = new QPushButton(this);
    btn1->resize(w / 2, h / 8);
    btn1->move(w / 4, h / 16 * 1);
    btn1->setText("单人模式");
    connect(btn1, &QPushButton::clicked, [=] {
        if (game1) delete game1;
        game1 = new single;
        game1->show();
    });
    QPushButton* btn2 = new QPushButton(this);
    btn2->resize(w / 2, h / 8);
    btn2->move(w / 4, h / 16 * 4);
    btn2->setText("双人模式");
    connect(btn2, &QPushButton::clicked, [=] {
        if (game2) delete game2;
        game2 = new twoplayer;
        game2->show();
    });
    QPushButton* btn3 = new QPushButton(this);
    btn3->resize(w / 2, h / 8);
    btn3->move(w / 4, h / 16 * 7);
    btn3->setText("载入单人游戏");
    connect(btn3, &QPushButton::clicked, [=] {
        game1 = new single;
        game1->pausepage->load->click();
    });
    QPushButton* btn4 = new QPushButton(this);
    btn4->resize(w / 2, h / 8);
    btn4->move(w / 4, h / 16 * 10);
    btn4->setText("载入双人游戏");
    connect(btn4, &QPushButton::clicked, [=] {
        game2 = new twoplayer;
        game2->pausepage->load->click();
    });
    QPushButton* exit = new QPushButton(this);
    exit->resize(w / 2, h / 8);
    exit->move(w / 4, h / 16 * 13);
    exit->setText("退出游戏");
    connect(exit, &QPushButton::clicked, [=] {
        if (game1) game1->close();
        if (game2) game2->close();
        this->close();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
