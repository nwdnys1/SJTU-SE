#include "twoplayer.h"
#include "ui_twoplayer.h"

#include <QDebug>
#include<QMessageBox>

twoplayer::twoplayer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::twoplayer)
{
    ui->setupUi(this);
    //这个类是双人游戏类，实现方法基本与单人一样，角色采用了数组的形式，每一个需要区分角色的函数都多加了一个int参数来告诉函数是哪一个角色调用了此函数，双人情况下0是玩家一，1是玩家二；
    //下面的初始化与单人游戏基本一样；
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    this->setFont(font);
    setFocusPolicy(Qt::StrongFocus);//不加此条代码时会导致上下左右键无法使用，具体原理我也不是很懂；
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    m = new map(this);
    end = new endpage;
    end->setFixedSize(pixwidth / 3, pixheight / 3);
    end->setWindowTitle("游戏结束");
    mapx = pixwidth / 2 - m->width * m->boxsize / 2;
    mapy = pixheight / 2 - m->height * m->boxsize / 2;
    mappos = { mapx,mapy };
    m->move(mapx, mapy);
    boundary = { mapx,mapy,m->width * m->boxsize,m->height * m->boxsize };
    for (int i = 0; i < 2; ++i) {
        A[i] = new character;
    }
    pausepage = new pausescreen;
    QString str;
    archives[0].setFileName("../Qtlink/save/archive4.txt");
    archives[1].setFileName("../Qtlink/save/archive5.txt");
    archives[2].setFileName("../Qtlink/save/archive6.txt");
    save_archive = new QDialog;
    save_archive->setFont(font);
    save_archive->setFixedSize(pixwidth / 4, pixheight / 4);
    savebtn = new QPushButton[3];
    for (int i = 0; i < 3; i++) {
        savebtn[i].setParent(save_archive);
        savebtn[i].setText("存档" + str.number(i + 1));
        savebtn[i].resize(QSize(pixwidth / 8, pixheight / 20));
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
        archivebtn[i].resize(QSize(pixwidth / 8, pixheight / 20));
    }
    archivebtn[0].move(pixwidth / 16, pixheight / 40);
    archivebtn[1].move(pixwidth / 16, pixheight / 40 * 4);
    archivebtn[2].move(pixwidth / 16, pixheight / 40 * 7);
    help = new QPushButton("帮助", this);
    help->move(10, 10);
    pause = new QPushButton("暂停", this);
    pause->move(pixwidth - 90, 10);
    this->setFixedSize(pixwidth, pixheight);
    this->setWindowTitle("双人模式");
    m->mapshuffle();
    for (int i = 0; i < 2; ++i) {
        A[i]->pos = m->randompos(A[i]->size) + QPoint(mapx, mapy);
    }
    while (!findsol()) {
        m->mapshuffle();
        for (int i = 0; i < 2; ++i) {
            A[i]->pos = m->randompos(A[i]->size) + QPoint(mapx, mapy);
        }
    }
    QTimer* timer = new QTimer(this);
    timer->start(10);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&twoplayer::update));
    connect(timer, &QTimer::timeout, this, [=] {
        keyboarddetect();
    });
    connect(timer, &QTimer::timeout, [=] {
        this->countdown -= 10;
        if (countdown == 0) {
            this->close();
            end->text->setText("倒计时结束！\n玩家1的分数:" + str.number(score[0]) + '\n' + "玩家2的分数:" + str.number(score[1]));
            game_end();
        }
        for (int i = 0; i < 2; ++i) {
            if (removing[i] && cnt[i] < removingtime) {
                cnt[i] += 1;
            }
            if (cnt[i] == removingtime) {
                cnt[i] = 0;
                removing[i] = false;
                chosen[i]->type = 0;
                chosen2[i]->type = 0;
                chosen[i]->activated = 0;
                chosen[i] = NULL;
                chosen2[i]->activated = 0;
                chosen2[i] = NULL;
                activated[i] = 0;
                if (!findsol()) {
                    this->close();
                    end->text->setText("没有可消除方块了！\n玩家1的分数:" + str.number(score[0]) + '\n' + "玩家2的分数:" + str.number(score[1]));
                    game_end();
                }
            }
        }
        if (hinting && hintcnt < hinttime) {
            hintcnt += 1;
        }
        if (hintcnt == hinttime) {
            hintcnt = 0;
            hinting = false;
        }
        for (int i = 0; i < 2; ++i) {
            if (freezing[i] && freezecnt[i] < freezetime) {
                freezecnt[i] += 1;
            }
            if (freezecnt[i] == freezetime) {
                freezecnt[i] = 0;
                freezing[i] = false;
            }
            if (dizzy[i] && dizzycnt[i] < dizzytime) {
                dizzycnt[i] += 1;
            }
            if (dizzycnt[i] == dizzytime) {
                dizzycnt[i] = 0;
                dizzy[i] = false;
            }
        }
    });
    connect(help, &QPushButton::clicked, [=] {
        QImage* img = new QImage;
        bool ld = img->load(":/new/prefix1/help2.png");
        QLabel* h = new QLabel;
        h->resize(img->width(), img->height());
        h->setPixmap(QPixmap::fromImage(*img));
        h->show();
    });
    connect(pause, &QPushButton::clicked, [=] {
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
    connect(pausepage->resume, &QPushButton::clicked, [=] {
        timer->start();
        pausepage->close();
    });
    connect(pausepage->quit, &QPushButton::clicked, [=] {
        pausepage->close();
        this->close();
    });
    connect(pausepage->save, &QPushButton::clicked, [=] {
        save_archive->exec();
    });
    for (int i = 0; i < 3; ++i) {
        connect(&savebtn[i], &QPushButton::clicked, [=] {
            if (archives[i].open(QIODevice::WriteOnly)) {
                QTextStream stream(&archives[i]);
                for (int i = 0; i < m->width * m->height; ++i) {
                    stream << m->boxes[i].type << "\n";
                }
                for (int j = 0; j < 2; ++j) {
                    stream << A[j]->pos.x() << "\n";
                    stream << A[j]->pos.y() << "\n";
                }
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
                for (int j = 0; j < 2; ++j) {
                    stream << score[j] << "\n";
                }
                stream << countdown << "\n";
                stream << hinting << "\n";
                stream << hintcnt << "\n";
                stream << solution1->pos.x() << "\n";
                stream << solution1->pos.y() << "\n";
                stream << solution2->pos.x() << "\n";
                stream << solution2->pos.y() << "\n";
                archives[i].close();
                QMessageBox::StandardButton suc = QMessageBox::information(save_archive, "提示", "保存成功");
            }
            });
    }
    connect(pausepage->load, &QPushButton::clicked, [=] {
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
                for (int j = 0; j < 2; ++j) {
                    in >> data;
                    A[j]->pos.rx() = data;
                    in >> data;
                    A[j]->pos.ry() = data;
                }

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
                for (int j = 0; j < 2; ++j) {
                    in >> data;
                    score[j] = data;
                }
                in >> data;
                countdown = data;
                in >> data;
                hinting = data;
                in >> data;
                hintcnt = data;
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

twoplayer::~twoplayer()
{
    delete ui;
}

void twoplayer::paintEvent(QPaintEvent*) {//绘制与单人模式基本一样；
    QPainter painter(this);
    if (hinting) {
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(Qt::red);
        QRect rec1(solution1->pos * m->boxsize + mappos, solution1->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy));
        painter.drawRect(rec1);
        QRect rec2(solution2->pos * m->boxsize + mappos, solution2->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy));
        painter.drawRect(rec2);
    }
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::transparent);
    painter.drawRect(boundary);
    painter.setPen(QPen(QColor(0, 0, 0, 122), 2));
    painter.setBrush(QColor(0, 0, 0, 122));
    if (chosen[0] && chosen[0]->type) painter.drawRect(QRect(chosen[0]->pos * m->boxsize + QPoint(mapx, mapy), chosen[0]->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));
    if (chosen2[0] && chosen2[0]->type) painter.drawRect(QRect(chosen2[0]->pos * m->boxsize + QPoint(mapx, mapy), chosen2[0]->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));
    painter.setPen(QPen(QColor(0, 255, 0, 122), 2));
    painter.setBrush(QColor(0, 255, 0, 122));
    if (chosen[1] && chosen[1]->type) painter.drawRect(QRect(chosen[1]->pos * m->boxsize + QPoint(mapx, mapy), chosen[1]->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));
    if (chosen2[1] && chosen2[1]->type) painter.drawRect(QRect(chosen2[1]->pos * m->boxsize + QPoint(mapx, mapy), chosen2[1]->pos * m->boxsize + QPoint(m->boxsize - 1 + mapx, m->boxsize - 1 + mapy)));
    painter.setPen(QColor(0, 0, 0, 160));
    painter.setBrush(QColor(0, 0, 0, 160));
    painter.drawEllipse(A[0]->pos, A[0]->size, A[0]->size);
    painter.setPen(QColor(0, 255, 0, 160));
    painter.setBrush(QColor(0, 255, 0, 160));
    painter.drawEllipse(A[1]->pos, A[1]->size, A[1]->size);
    for (int i = 0; i < m->plus1s_num; ++i) {
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
    for (int i = 0; i < m->freeze_num; ++i) {
        painter.setPen(QColor("aqua"));
        painter.setBrush(QColor("aqua"));
        painter.drawEllipse(m->fr[i].pos + mappos, m->fr[i].size, m->fr[i].size);
    }
    for (int i = 0; i < m->diz_num; ++i) {
        painter.setPen(QColor("brown"));
        painter.setBrush(QColor("brown"));
        painter.drawEllipse(m->dz[i].pos + mappos, m->dz[i].size, m->dz[i].size);
    }
    QString str;
    painter.setPen(Qt::black);
    painter.drawText(0, 0, pixwidth, mapy, Qt::AlignCenter, "剩余时间：" + str.number(countdown / 1000));
    painter.drawText(0, 0, pixwidth / 2, mapy, Qt::AlignCenter, "玩家1的分数:" + str.number(score[0]));
    painter.drawText(pixwidth / 2, 0, pixwidth / 2, mapy, Qt::AlignCenter, "玩家2的分数:" + str.number(score[1]));
    if (hinting) painter.drawText(pixwidth / 2, 0, pixwidth / 4, mapy, Qt::AlignCenter, "剩余提示时间:" + str.number(hinttime / 100 - hintcnt / 100));
    if (freezing[0]) painter.drawText(0, mapy + 30, mapx, 60, Qt::AlignCenter, "玩家1被冻结!\n剩余时间:" + str.number(freezetime / 100 - freezecnt[0] / 100));
    if (freezing[1]) painter.drawText(pixwidth - mapx, mapy + 30, mapx, 60, Qt::AlignCenter, "玩家2被冻结!\n剩余时间:" + str.number(freezetime / 100 - freezecnt[1] / 100));
    if (dizzy[0]) painter.drawText(0, mapy + 90, mapx, 60, Qt::AlignCenter, "玩家1晕头转向!\n剩余时间:" + str.number(dizzytime / 100 - dizzycnt[0] / 100));
    if (dizzy[1]) painter.drawText(pixwidth - mapx, mapy + 90, mapx, 60, Qt::AlignCenter, "玩家2晕头转向!\n剩余时间:" + str.number(dizzytime / 100 - dizzycnt[1] / 100));
    for (int i = 0; i < 2; ++i) {
        if (removing[i]) {
            painter.setPen(QPen(Qt::black, 2));
            painter.setBrush(Qt::black);
            switch (line[i]) {
            case(1): {
                painter.drawLine(point1[i], point4[i]);
                break;
            }
            case(2): {
                painter.drawLine(point1[i], point2[i]);
                painter.drawLine(point2[i], point4[i]);
                break;
            }
            case(3): {
                painter.drawLine(point1[i], point2[i]);
                painter.drawLine(point2[i], point3[i]);
                painter.drawLine(point3[i], point4[i]);
                break;
            }
            }
        }
    }
}

void twoplayer::judge(box* tmp, int i) {//与单人模式基本一样；
    if (!removing[i]) {
        if (activated[i]) {
            if (tmp == chosen[i]) return;
            if (link(chosen[i], tmp, i)) {
                removing[i] = true;
                chosen2[i] = tmp;
                chosen2[i]->activated = 1;
                score[i] += 1;
            }
            else {
                chosen[i]->activated = 0;
                chosen[i] = NULL;
                activated[i] = 0;
            }
        }
        else {
            chosen[i] = tmp;
            chosen[i]->activated = 1;
            activated[i] = 1;
        }
    }
}

void twoplayer::keyPressEvent(QKeyEvent* event) {//多加了上下左右按键；
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
    case(Qt::Key_Up): {
        Press_UP = true;
        break;
    }
    case(Qt::Key_Down): {
        Press_DOWN = true;
        break;
    }
    case(Qt::Key_Left): {
        Press_LEFT = true;
        break;
    }
    case(Qt::Key_Right): {
        Press_RIGHT = true;
        break;
    }
    }
}

void twoplayer::keyReleaseEvent(QKeyEvent* event) {
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
    case(Qt::Key_Up): {
        Press_UP = false;
        break;
    }
    case(Qt::Key_Down): {
        Press_DOWN = false;
        break;
    }
    case(Qt::Key_Left): {
        Press_LEFT = false;
        break;
    }
    case(Qt::Key_Right): {
        Press_RIGHT = false;
        break;
    }
    }
}

bool twoplayer::link1(QPoint a, QPoint b, int i) {
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
            point1[i] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[i] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
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
            point1[i] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[i] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        }
        return flag;
    }

}

bool twoplayer::link2(QPoint a, QPoint b, int i) {
    QPoint c = { a.x(),b.y() };
    QPoint d = { b.x(),a.y() };
    if (link1(a, c, i) && link1(c, b, i) && !m->boxes[c.y() * m->width + c.x()].type) {
        point1[i] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point4[i] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point2[i] = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        return true;
    }
    if (link1(a, d, i) && link1(d, b, i) && !m->boxes[d.y() * m->width + d.x()].type) {
        point1[i] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point4[i] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        point2[i] = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
        return true;
    }
    return false;
}

bool twoplayer::link3(QPoint a, QPoint b, int cha) {
    for (int i = a.y() + 1; i < m->height; ++i) {
        if (i == b.y()) continue;
        if (m->boxes[i * m->width + a.x()].type) break;
        QPoint c = { a.x(),i };
        if (link1(a, c, cha) && link2(b, c, cha)) {
            QPoint d = { b.x(),i };
            point1[cha] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[cha] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2[cha] = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3[cha] = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.y() - 1; i >= 0; --i) {
        if (i == b.y()) continue;
        if (m->boxes[i * m->width + a.x()].type) break;
        QPoint c = { a.x(),i };
        if (link1(a, c, cha) && link2(b, c, cha)) {
            QPoint d = { b.x(),i };
            point1[cha] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[cha] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2[cha] = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3[cha] = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.x() + 1; i < m->width; ++i) {
        if (i == b.x()) continue;
        if (m->boxes[a.y() * m->width + i].type) break;
        QPoint c = { i,a.y() };
        if (link1(a, c, cha) && link2(b, c, cha)) {
            QPoint d = { i,b.y() };
            point1[cha] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[cha] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2[cha] = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3[cha] = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    for (int i = a.x() - 1; i >= 0; --i) {
        if (i == b.x()) continue;
        if (m->boxes[a.y() * m->width + i].type) break;
        QPoint c = { i,a.y() };
        if (link1(a, c, cha) && link2(b, c, cha)) {
            QPoint d = { i,b.y() };
            point1[cha] = a * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point4[cha] = b * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point2[cha] = c * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            point3[cha] = d * m->boxsize + QPoint(m->boxsize / 2 + mapx, m->boxsize / 2 + mapy);
            return true;
        }
    }
    return false;
}

bool twoplayer::link(const box* a, const box* b, int i) {
    if (a->type == b->type && a->pos != b->pos) {
        if (a->pos.x() == b->pos.x() || a->pos.y() == b->pos.y()) {
            if (link1(a->pos, b->pos, i)) {
                line[i] = 1;
                return true;
            }
            if (link3(a->pos, b->pos, i)) {
                line[i] = 3;
                return true;
            }
        }
        else {
            if (link2(a->pos, b->pos, i)) {
                line[i] = 2;
                return true;
            }
            if (link3(a->pos, b->pos, i)) {
                line[i] = 3;
                return true;
            }
        }
    }
    return false;
}

bool twoplayer::findsol() {
    for (int i = 0; i < m->width * m->height; ++i) {
        if (!m->boxes[i].type) continue;
        for (int j = i + 1; j < m->width * m->height; ++j) {
            if (m->boxes[i].type == m->boxes[j].type) {
                int cha[2];
                for (int i = 0; i < 2; ++i) {
                    cha[i] = (A[i]->pos - mappos).y() / m->boxsize * m->width + (A[i]->pos - mappos).x() / m->boxsize;
                }
                if (link(&m->boxes[i], &m->boxes[j], 0) && m->connectable(cha[0], i) && m->connectable(cha[0], j) && m->connectable(cha[1], i) && m->connectable(cha[1], j)) {
                    solution1 = &m->boxes[i];
                    solution2 = &m->boxes[j];
                    return true;
                }
            }
        }
    }
    return false;
}

void twoplayer::game_end() {
    end->text->setGeometry(0, 0, end->width(), end->height() / 2);
    end->text->setAlignment(Qt::AlignCenter);
    end->return_menu->resize(end->width() / 4, end->height() / 6);
    end->return_menu->move(end->width() / 8 * 3, end->height() / 8 * 5);
    end->exec();
}

bool twoplayer::collide(QPoint a, QPoint b, int r) {
    return ((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y()) <= r * r);
}

void twoplayer::move(QPoint p, int i) {
    if (freezing[i]) return;
    if (dizzy[i]) p *= -1;
    QPoint cur = A[i]->pos + p * A[i]->size;
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
        judge(tmp, i);
    }
    A[i]->pos = cur - p - p * A[i]->size;
    use_tool(i);
}

void twoplayer::keyboarddetect() {
    if (Press_W) move({ 0,-1 }, 0);
    if (Press_S) move({ 0,1 }, 0);
    if (Press_A) move({ -1,0 }, 0);
    if (Press_D) move({ 1,0 }, 0);
    if (Press_UP) move({ 0,-1 }, 1);
    if (Press_DOWN) move({ 0,1 }, 1);
    if (Press_LEFT) move({ -1,0 }, 1);
    if (Press_RIGHT) move({ 1,0 }, 1);
}

void twoplayer::use_tool(int cha) {
    for (int i = 0; i < m->plus1s_num; ++i) {
        if (collide(A[cha]->pos, m->pl[i].pos + mappos, m->pl[i].size + A[cha]->size)) {
            countdown += 30000;
            m->pl[i].pos = m->randompos(m->pl[i].size);
        }
    }
    for (int i = 0; i < m->shuffle_num; ++i) {
        if (collide(A[cha]->pos, m->sf[i].pos + mappos, m->sf[i].size + A[cha]->size)) {
            for (int i = 0; i < 2; ++i) {
                removing[i] = false;
                if (chosen[i]) chosen[i]->activated = 0;
                chosen[i] = NULL;
                if (chosen2[i]) chosen2[i]->activated = 0;
                chosen2[i] = NULL;
                activated[i] = 0;
                cnt[i] = 0;
            }
            m->mapshuffle();
            for (int i = 0; i < 2; ++i) {
                A[i]->pos = m->randompos(m->sf[i].size) + mappos;
            }
            while (!findsol()) {
                m->mapshuffle();
                for (int i = 0; i < 2; ++i) {
                    A[i]->pos = m->randompos(m->sf[i].size) + mappos;
                }
            }
            use_tool(cha);
        }
    }
    for (int i = 0; i < m->hint_num; ++i) {
        if (collide(A[cha]->pos, m->ht[i].pos + mappos, m->ht[i].size + A[cha]->size)) {
            hinting = true;
            hintcnt = 0;
            m->ht[i].pos = m->randompos(m->ht[i].size);
        }
    }
    for (int i = 0; i < m->freeze_num; ++i) {
        if (collide(A[cha]->pos, m->fr[i].pos + mappos, m->fr[i].size + A[cha]->size)) {
            freezing[1 - cha] = true;
            freezecnt[1 - cha] = 0;
            m->fr[i].pos = m->randompos(m->fr[i].size);
        }
    }
    for (int i = 0; i < m->diz_num; ++i) {
        if (collide(A[cha]->pos, m->dz[i].pos + mappos, m->dz[i].size + A[cha]->size)) {
            dizzy[1 - cha] = true;
            dizzycnt[1 - cha] = 0;
            m->dz[i].pos = m->randompos(m->dz[i].size);
        }
    }
}
