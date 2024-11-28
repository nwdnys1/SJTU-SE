#include "pausescreen.h"
#include "ui_pausescreen.h"
#include<QPushButton>
pausescreen::pausescreen(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::pausescreen)
{
    ui->setupUi(this);
    //这个类是暂停界面类，下面的代码进行了ui的初始化以及按钮的新建；
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    this->setFont(font);
    this->setWindowTitle("暂停");
    save = new QPushButton("保存", this);
    load = new QPushButton("载入", this);
    resume = new QPushButton("继续", this);
    quit = new QPushButton("退出", this);
}

pausescreen::~pausescreen()
{
    delete ui;
}

