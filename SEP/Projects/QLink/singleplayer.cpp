#include "singleplayer.h"
#include "ui_singleplayer.h"

#include <QDebug>
#include <QMessageBox>

single::single(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::single)
{
    ui->setupUi(this);
    //这个类是单人游戏类，用于实现单人游戏，下面是一些初始化，包括对各个界面的初始化；
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    this->setFont(font);
    m = new map(this);
    end = new endpage;
    end->setFixedSize(pixwidth / 3, pixheight / 3);
    end->setWindowTitle("游戏结束");
    mapx = pixwidth / 2 - m->width * m->boxsize / 2;//这两个变量存储了游戏地图与整个游戏界面位置的差异，在后续绘制会用到；
    mapy = pixheight / 2 - m->height * m->boxsize / 2;
    mappos = { mapx,mapy };
    m->move(mapx, mapy);
    boundary = { mapx,mapy,m->width * m->boxsize,m->height * m->boxsize };//游戏的边界，角色不能超出边界；
    A = new character;
    pausepage = new pausescreen;
    QString str;//这个变量用于后面drawtext，方便直接调用number函数；
    archives[0].setFileName("../Qtlink/save/archive1.txt");//图片资源以及保存、加载界面的初始化，总共有三个存档位；
    archives[1].setFileName("../Qtlink/save/archive2.txt");
    archives[2].setFileName("../Qtlink/save/archive3.txt");
    savebtn = new QPushButton[3];
    save_archive = new QDialog;
    save_archive->setFont(font);
    save_archive->setFixedSize(pixwidth / 4, pixheight / 4);
    for (int i = 0; i < 3; i++) {
        savebtn[i].setParent(save_archive);
        savebtn[i].resize(pixwidth / 8, pixheight / 20);
        savebtn[i].setText("存档" + str.number(i + 1));
    }
    savebtn[0].move(pixwidth / 16, pixheight / 40);
    savebtn[1].move(pixwidth / 16, pixheight / 40 * 4);
    savebtn[2].move(pixwidth / 16, pixheight / 40 * 7);
    load_archive = new QDialog;
    load_archive->setFont(font);
    load_archive->setFixedSize(pixwidth / 4, pixheight / 4);
    archivebtn = new QPushButton[3];
    for (int i = 0; i < 3; i++) {
        archivebtn[i].setParent(load_archive);
        archivebtn[i].setText("存档" + str.number(i + 1));
        archivebtn[i].resize(pixwidth / 8, pixheight / 20);
    }
    archivebtn[0].move(pixwidth / 16, pixheight / 40);
    archivebtn[1].move(pixwidth / 16, pixheight / 40 * 4);
    archivebtn[2].move(pixwidth / 16, pixheight / 40 * 7);
    help = new QPushButton("帮助", this);//帮助功能
    help->move(10, 10);
    pause = new QPushButton("暂停", this);//暂停功能；
    pause->move(pixwidth - 90, 10);
    this->setFixedSize(pixwidth, pixheight);
    this->setWindowTitle("单人模式");
    QTimer* timer = new QTimer(this);
    m->mapshuffle();//生成地图以及角色位置，如果地图无解或者角色生成在了封闭内部就重新生成；
    A->pos = m->randompos(A->size) + mappos;
    while (!findsol()) {
        m->mapshuffle();
        A->pos = m->randompos(A->size) + mappos;
    }
    timer->start(10);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&single::update));
    connect(timer, &QTimer::timeout, this, [=] {//每10ms检测一次键盘输入；
        keyboarddetect();
    });
    connect(timer, &QTimer::timeout, [=] {//这个lamda函数主要用于各种计时，包括游戏倒计时，消除方块动画的计时以及各个道具效果的计时；
        this->countdown -= 10;
        if (countdown == 0) {//游戏倒计时结束；
            this->close();
            end->text->setText("倒计时结束！\n你的分数:" + str.number(score));
            game_end();
        }
        if (removing && cnt < removetime) {
            cnt += 1;
        }
        if (cnt == removetime) {//消除倒计时结束，方块消除且选中的方块都清空，每次消除都需要判断地图是否有解；
            cnt = 0;
            removing = false;
            chosen->type = 0;
            chosen2->type = 0;
            chosen->activated = 0;
            chosen = NULL;
            chosen2->activated = 0;
            chosen2 = NULL;
            activated = 0;
            if (!findsol()) {
                this->close();
                end->text->setText("没有可消除方块了！\n你的分数:" + str.number(score));
                game_end();
            }
        }
        if (hinting && hintcnt < hinttime) {//hint与flash的效果倒计时
            hintcnt += 1;
        }
        if (hintcnt == hinttime) {
            hintcnt = 0;
            hinting = false;
        }
        if (flashing && flashcnt < flashtime) {
            flashcnt += 1;
        }
        if (flashcnt == flashtime) {
            flashcnt = 0;
            flashing = false;
        }
    });
    connect(help, &QPushButton::clicked, [=] {//帮助按钮，按下显示帮助
        QImage* img = new QImage;
        bool ld = img->load(":/new/prefix1/help1.png");
        QLabel* h = new QLabel;
        h->resize(img->width(), img->height());
        h->setPixmap(QPixmap::fromImage(*img));
        h->show();
    });
    connect(pause, &QPushButton::clicked, [=] {//暂停按钮，按下显示暂停界面；
        timer->stop();
        pausepage->setFixedSize(pixwidth / 4, pixheight / 4);
        pausepage->save->resize(pixwidth / 8, pixheight / 20);
        pausepage->load->resize(pixwidth / 8, pixheight / 20);
        pausepage->resume->resize(pixwidth / 8, pixheight / 20);
        pausepage->quit->resize(pixwidth / 8, pixheight / 20);
        pausepage->save->move(pixwidth / 16, pixheight / 52);
        pausepage->load->move(pixwidth / 16, pixheight / 52 * 4);
        pausepage->resume->move(pixwidth / 16, pixheight / 52 * 7);
        pausepage->quit->move(pixwidth / 16, pixheight / 52 * 10);
        pausepage->exec();
    });
    connect(pausepage->resume, &QPushButton::clicked, [=] {//继续按钮
        timer->start();
        pausepage->close();
    });
    connect(pausepage->quit, &QPushButton::clicked, [=] {//退出按钮
        pausepage->close();
        this->close();
    });
    connect(pausepage->save, &QPushButton::clicked, [=] {//保存按钮，每一个按钮绑定一个lamda函数，将当前的所有信息输入到文档中；
        save_archive->exec();
    });
    for (int i = 0; i < 3; ++i) {
        connect(&savebtn[i], &QPushButton::clicked, [=] {
            if (archives[i].open(QIODevice::WriteOnly)) {
                QTextStream stream(&archives[i]);
                for (int i = 0; i < m->width * m->height; ++i) {
                    stream << m->boxes[i].type << "\n";
                }
                stream << A->pos.x() << "\n";
                stream << A->pos.y() << "\n";
                for (int i = 0; i < m->plus1s_num; ++i) {
                    stream << m->pl[i].pos.x() << "\n";
                    stream << m->pl[i].pos.y() << "\n";
                }
                for (int i = 0; i < m->shuffle_num; ++i) {
                    stream << m->sf[i].pos.x() << "\n";
                    stream << m->sf[i].pos.y() << "\n";
                }
                for (int i = 0; i < m->hint_num; ++i) {
                    stream << m->ht[i].pos.x() << "\n";
                    stream << m->ht[i].pos.y() << "\n";
                }
                for (int i = 0; i < m->flash_num; ++i) {
                    stream << m->fl[i].pos.x() << "\n";
                    stream << m->fl[i].pos.y() << "\n";
                }
                stream << score << "\n";
                stream << countdown << "\n";
                stream << hinting << "\n";
                stream << hintcnt << "\n";
                stream << flashing << "\n";
                stream << flashcnt << "\n";
                stream << solution1->pos.x() << "\n";
                stream << solution1->pos.y() << "\n";
                stream << solution2->pos.x() << "\n";
                stream << solution2->pos.y() << "\n";
                archives[i].close();
                QMessageBox::StandardButton suc = QMessageBox::information(save_archive, "提示", "保存成功");
            }
        });
    }
    connect(pausepage->load, &QPushButton::clicked, [=] {//加载按钮，原理同保存；
        load_archive->exec();
    });
    for (int i = 0; i < 3; ++i) {
        connect(&archivebtn[i], &QPushButton::clicked, [=] {
            if (archives[i].open(QIODevice::ReadOnly)) {
                QTextStream in(&archives[i]);
                QString line;
                int data;
                for (int i = 0; i < m->width * m->height; ++i) {
                    in >> data;
                    m->boxes[i].type = data;
                }
                in >> data;
                A->pos.rx() = data;
                in >> data;
                A->pos.ry() = data;
                for (int i = 0; i < m->plus1s_num; ++i) {
                    in >> data;
                    m->pl[i].pos.rx() = data;
                    in >> data;
                    m->pl[i].pos.ry() = data;
                }
                for (int i = 0; i < m->shuffle_num; ++i) {
                    in >> data;
                    m->sf[i].pos.rx() = data;
                    in >> data;
                    m->sf[i].pos.ry() = data;
                }
                for (int i = 0; i < m->hint_num; ++i) {
                    in >> data;
                    m->ht[i].pos.rx() = data;
                    in >> data;
                    m->ht[i].pos.ry() = data;
                }
                for (int i = 0; i < m->flash_num; ++i) {
                    in >> data;
                    m->fl[i].pos.rx() = data;
                    in >> data;
                    m->fl[i].pos.ry() = data;
                }
                in >> data;
                score = data;
                in >> data;
                countdown = data;
                in >> data;
                hinting = data;
                in >> data;
                hintcnt = data;
                in >> data;
                flashing = data;
                in >> data;
                flashcnt = data;
                int x, y;
                in >> x >> y;
                solution1 = &m->boxes[y * m->width + x];
                in >> x >> y;
                solution2 = &m->boxes[y * m->width + x];
                archives[i].close();
                this->show();
                load_archive->close();
                pausepage->close();
                timer->start();
            }
        });
    }
}

single::~single()
{
    delete ui;
}

void single::paintEvent(QPaintEvent*) {//绘制函数
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//抗锯齿开启；
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    painter.setFont(font);
    if (hinting) {//提示状态下，将两个解高亮显示，即红色框框；
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(Qt::red);
        QRect rec1(solution1->pos * m->boxsize + QPoint(mapx, mapy), solution1->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy));
        painter.drawRect(rec1);
        QRect rec2(solution2->pos * m->boxsize + QPoint(mapx, mapy), solution2->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy));
        painter.drawRect(rec2);
    }
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::transparent);
    painter.drawRect(boundary);//绘制边界；
    if (chosen) painter.drawRect(QRect(chosen->pos * m->boxsize + QPoint(mapx, mapy), chosen->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));//绘制选中的方块，为黑色框框
    if (chosen2) painter.drawRect(QRect(chosen2->pos * m->boxsize + QPoint(mapx, mapy), chosen2->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));
    painter.setBrush(Qt::black);
    painter.drawEllipse(A->pos, A->size, A->size);//绘制角色，为黑色实心圆；
    for (int i = 0; i < m->plus1s_num; ++i) {//绘制各个道具；
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(m->pl[i].pos + mappos, m->pl[i].size, m->pl[i].size);
    }
    for (int i = 0; i < m->shuffle_num; ++i) {
        painter.setPen(Qt::yellow);
        painter.setBrush(Qt::yellow);
        painter.drawEllipse(m->sf[i].pos + mappos, m->sf[i].size, m->sf[i].size);
    }
    for (int i = 0; i < m->hint_num; ++i) {
        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(m->ht[i].pos + mappos, m->ht[i].size, m->ht[i].size);
    }
    for (int i = 0; i < m->flash_num; ++i) {
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
        painter.drawEllipse(m->fl[i].pos + mappos, m->fl[i].size, m->fl[i].size);
    }
    QString str;
    painter.setPen(Qt::black);//绘制各个文本，包括倒计时、分数、道具时间；
    painter.drawText(0, 0, pixwidth, mapy, Qt::AlignCenter, "剩余时间：" + str.number(countdown / 1000));
    painter.drawText(pixwidth / 2, 0, pixwidth / 2, mapy, Qt::AlignCenter, "分数:" + str.number(score));
    if (hinting) painter.drawText(pixwidth / 4, 0, pixwidth / 4, mapy, Qt::AlignCenter, "剩余提示时间:" + str.number(hinttime / 100 - hintcnt / 100));
    if (flashing) painter.drawText(pixwidth / 2, 0, pixwidth / 4, mapy, Qt::AlignCenter, "剩余闪现时间:" + str.number(flashtime / 100 - flashcnt / 100));
    if (removing) {//绘制消除时方块的连线，line代表需要几条折线；
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(Qt::black);
        switch (line) {
        case(1): {
            painter.drawLine(point1, point4);
            break;
        }
        case(2): {
            painter.drawLine(point1, point2);
            painter.drawLine(point2, point4);
            break;
        }
        case(3): {
            painter.drawLine(point1, point2);
            painter.drawLine(point2, point3);
            painter.drawLine(point3, point4);
            break;
        }
        }
    }
}

void single::judge(box* tmp) {//这个函数用于判断选中方块应该干什么，如果没有其他选中方块，则选中它，如果有，则判断是否可以消除；
    if (!removing) {
        if (activated) {
            if (tmp == chosen) return;
            if (link(chosen, tmp)) {
                removing = true;
                chosen2 = tmp;
                chosen2->activated = 1;
                score += 1;
            }
            else {
                chosen->activated = 0;
                chosen = NULL;
                activated = 0;
            }
        }
        else {
            chosen = tmp;
            chosen->activated = 1;
            activated = 1;
        }
    }
}

void single::keyPressEvent(QKeyEvent* event) {//移动，用数组实现
    switch (event->key()) {
    case(Qt::Key_W): {
        Press_W = true;
        break;
    }
    case(Qt::Key_S): {
        Press_S = true;
        break;
    }
    case(Qt::Key_A): {
        Press_A = true;
        break;
    }
    case(Qt::Key_D): {
        Press_D = true;
        break;
    }
    }

}

void single::keyReleaseEvent(QKeyEvent* event) {
    switch (event->key()) {
    case(Qt::Key_W): {
        Press_W = false;
        break;
    }
    case(Qt::Key_S): {
        Press_S = false;
        break;
    }
    case(Qt::Key_A): {
        Press_A = false;
        break;
    }
    case(Qt::Key_D): {
        Press_D = false;
        break;
    }
    }
}

void single::mousePressEvent(QMouseEvent* event) {//flash道具效果的实现；
    if (flashing && event->button() == Qt::LeftButton) {
        flashto(event->pos());
    }

}

bool single::link1(QPoint a, QPoint b) {//link1代表两个方块可以用一条折线连接，link2、link3以此类推；
    if (a.x() == b.x()) {
        int x = a.x();
        bool flag = true;
        int min, max;
        if (a.y() > b.y()) {
            max = a.y(); min = b.y();
        }
        else {
            max = b.y(); min = a.y();
        }
        for (int i = min + 1; i < max; ++i) {
            if (m->boxes[i * m->width + x].type) {
                flag = false;
                break;
            }
        }
        if (flag) {
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        }
        return flag;
    }
    else {
        int y = a.y();
        bool flag = true;
        int min, max;
        if (a.x() > b.x()) {
            max = a.x(); min = b.x();
        }
        else {
            max = b.x(); min = a.x();
        }
        for (int i = min + 1; i < max; ++i) {
            if (m->boxes[y * m->width + i].type) {
                flag = false;
                break;
            }
        }
        if (flag) {
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        }
        return flag;
    }
}

bool single::link2(QPoint a, QPoint b) {
    QPoint c = { a.x(),b.y() };
    QPoint d = { b.x(),a.y() };
    if (link1(a, c) && link1(c, b) && !m->boxes[c.y() * m->width + c.x()].type) {
        point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point2 = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        return true;
    }
    if (link1(a, d) && link1(d, b) && !m->boxes[d.y() * m->width + d.x()].type) {
        point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point2 = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        return true;
    }
    return false;
}

bool single::link3(QPoint a, QPoint b) {
    for (int i = a.y() + 1; i < m->height; ++i) {
        if (i == b.y()) continue;
        if (m->boxes[i * m->width + a.x()].type) break;
        QPoint c = { a.x(),i };
        if (link1(a, c) && link2(b, c)) {
            QPoint d = { b.x(),i };
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2 = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3 = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.y() - 1; i >= 0; --i) {
        if (i == b.y()) continue;
        if (m->boxes[i * m->width + a.x()].type) break;
        QPoint c = { a.x(),i };
        if (link1(a, c) && link2(b, c)) {
            QPoint d = { b.x(),i };
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2 = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3 = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.x() + 1; i < m->width; ++i) {
        if (i == b.x()) continue;
        if (m->boxes[a.y() * m->width + i].type) break;
        QPoint c = { i,a.y() };
        if (link1(a, c) && link2(b, c)) {
            QPoint d = { i,b.y() };
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2 = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3 = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.x() - 1; i >= 0; --i) {
        if (i == b.x()) continue;
        if (m->boxes[a.y() * m->width + i].type) break;
        QPoint c = { i,a.y() };
        if (link1(a, c) && link2(b, c)) {
            QPoint d = { i,b.y() };
            point1 = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4 = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2 = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3 = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }

    return false;
}

bool single::link(const box* a, const box* b) {//判断两个方块是否可以连接，逻辑是先判断两者位置关系，是否能用1或2条，然后判断能否用3条；
    if (a->type == b->type && a->pos != b->pos) {
        if (a->pos.x() == b->pos.x() || a->pos.y() == b->pos.y()) {
            if (link1(a->pos, b->pos)) {
                line = 1;
                return true;
            }
            if (link3(a->pos, b->pos)) {
                line = 3;
                return true;
            }
        }
        else {
            if (link2(a->pos, b->pos)) {
                line = 2;
                return true;
            }
            if (link3(a->pos, b->pos)) {
                line = 3;
                return true;
            }
        }
    }
    line = 0;//用于测试；
    return false;
}

bool single::findsol() {//判断当前地图是否有解，要求可以消除且方块是可以被角色选中的；
    for (int i = 0; i < m->width * m->height; ++i) {
        if (!m->boxes[i].type) continue;
        for (int j = i + 1; j < m->width * m->height; ++j) {
            if (m->boxes[i].type == m->boxes[j].type) {
                int cha = (A->pos - mappos).y() / m->boxsize * m->width + (A->pos - mappos).x() / m->boxsize;
                if (link(&m->boxes[i], &m->boxes[j]) && m->connectable(cha, i) && m->connectable(cha, j)) {
                    solution1 = &m->boxes[i];
                    solution2 = &m->boxes[j];
                    return true;
                }
            }
        }
    }
    return false;
}

void single::game_end() {//显示游戏结束界面；
    end->text->setGeometry(0, 0, end->width(), end->height() / 2);
    end->text->setAlignment(Qt::AlignCenter);
    end->return_menu->resize(end->width() / 4, end->height() / 6);
    end->return_menu->move(end->width() / 8 * 3, end->height() / 8 * 5);
    end->exec();

}

bool single::collide(QPoint a, QPoint b, int r) {//一个简单的函数判断两个圆是否碰撞，用于节省代码量；
    return ((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y()) <= r * r);
}

void single::move(QPoint p) {//移动函数，输入的qpoint代表移动方向的单位矢量，speed可以调节移动速度，逻辑是循环speed次的移动一个像素，每移动一个像素都需要判断是否撞到道具、方块或者边界；
    QPoint cur = A->pos + p * A->size;
    box* tmp = NULL;
    for (int i = 0; i < speed + 1; ++i) {
        cur += p;
        if (!boundary.contains(cur)) {
            break;
        }
        if (m->in_box(cur - mappos)) {
            tmp = &m->boxes[(cur.y() - mapy) / m->boxsize * m->width + (cur.x() - mapx) / m->boxsize];
            break;
        }
    }
    if (tmp) {
        judge(tmp);
    }
    A->pos = cur - p - p * A->size;
    use_tool();
}

void single::keyboarddetect() {//检测按键输入，进行移动；
    if (Press_W) move({ 0,-1 });
    if (Press_S) move({ 0,1 });
    if (Press_A) move({ -1,0 });
    if (Press_D) move({ 1,0 });
}

void single::flashto(QPoint p) {//实现闪现效果，需要判断点击位置是否可以到达，以及点击位置是方块时闪现到随机一面空地上；
    int cha = (A->pos - mappos).y() / m->boxsize * m->width + (A->pos - mappos).x() / m->boxsize;
    int des = (p - mappos).y() / m->boxsize * m->width + (p - mappos).x() / m->boxsize;
    if (m->connectable(cha, des) && boundary.contains(p)) {
        int cur = m->in_box(p - mappos);
        if (cur) {
            if (!m->boxes[cur + 1].type && m->connectable(cha, cur + 1)) {
                A->pos = mappos + m->boxes[cur].pos * m->boxsize + QPoint(m->boxsize + A->size, m->boxsize / 2);
                judge(&m->boxes[cur]);
                return;
            }
            if (!m->boxes[cur - 1].type && m->connectable(cha, cur - 1)) {
                A->pos = mappos + m->boxes[cur].pos * m->boxsize + QPoint(-A->size, m->boxsize / 2);
                judge(&m->boxes[cur]);
                return;
            }
            if (!m->boxes[cur + m->width].type && m->connectable(cha, cur + m->width)) {
                A->pos = mappos + m->boxes[cur].pos * m->boxsize + QPoint(m->boxsize / 2, m->boxsize + A->size);
                judge(&m->boxes[cur]);
                return;
            }
            if (!m->boxes[cur - m->width].type && m->connectable(cha, cur - m->width)) {
                A->pos = mappos + m->boxes[cur].pos * m->boxsize + QPoint(m->boxsize / 2, -A->size);
                judge(&m->boxes[cur]);
                return;
            }
        }
        else {
            A->pos = p;
        }
        use_tool();
    }
}

void single::use_tool() {//使用道具的函数，逻辑为碰撞到道具就实现对应的效果；
    for (int i = 0; i < m->plus1s_num; ++i) {
        if (collide(A->pos, m->pl[i].pos + mappos, m->pl[i].size + A->size)) {
            countdown += 30000;
            m->pl[i].pos = m->randompos(m->pl[i].size);
        }
    }
    for (int i = 0; i < m->shuffle_num; ++i) {
        if (collide(A->pos, m->sf[i].pos + mappos, m->sf[i].size + A->size)) {
            removing = false;
            if (chosen) chosen->activated = 0;
            chosen = NULL;
            if (chosen2) chosen2->activated = 0;
            chosen2 = NULL;
            activated = 0;
            cnt = 0;
            m->mapshuffle();
            A->pos = m->randompos(m->sf[i].size) + mappos;
            while (!findsol()) {
                m->mapshuffle();
                A->pos = m->randompos(m->sf[i].size) + mappos;
            }
            use_tool();
        }
    }
    for (int i = 0; i < m->hint_num; ++i) {
        if (collide(A->pos, m->ht[i].pos + mappos, m->ht[i].size + A->size)) {
            hinting = true;
            hintcnt = 0;
            m->ht[i].pos = m->randompos(m->ht[i].size);
        }
    }
    for (int i = 0; i < m->flash_num; ++i) {
        if (collide(A->pos, m->fl[i].pos + mappos, m->fl[i].size + A->size)) {
            flashing = true;
            flashcnt = 0;
            m->fl[i].pos = m->randompos(m->fl[i].size);
        }
    }
}

