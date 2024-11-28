#include "endpage.h"
#include "ui_endpage.h"

#include<QPushButton>

endpage::endpage(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::endpage)
{
    ui->setupUi(this);
    //这个类是游戏结束的界面类，下面的代码对此界面的ui进行了初始化，包括字体、大小、按钮等；
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    this->setFont(font);
    text = new QLabel(this);
    return_menu = new QPushButton(this);
    return_menu->setText("回到主菜单");
    connect(return_menu, &QPushButton::clicked, [=] {
        this->close();
    });
}

endpage::~endpage()
{
    delete ui;
}


