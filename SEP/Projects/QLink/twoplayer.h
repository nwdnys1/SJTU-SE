#pragma once
#ifndef TWOPLAYER_H
#define TWOPLAYER_H

#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QBrush>
#include <QPushButton>
#include <QPen>
#include <QFileDialog>
#include<pausescreen.h>
#include<endpage.h>
#include<map.h>

namespace Ui {
    class twoplayer;
}

class twoplayer : public QWidget
{
    Q_OBJECT
public:
    explicit twoplayer(QWidget* parent = nullptr);
    ~twoplayer();

    map* m = NULL;
    int pixwidth = 1600;
    int pixheight = 900;
    int mapx;
    int mapy;
    QPoint mappos;
    QRect boundary;
    character* A[2] = { NULL };
    int activated[2] = { 0 };//表示当前激活个数
    box* chosen[2] = { NULL };
    box* chosen2[2] = { NULL };
    int countdown = 1000 * 1000;
    QPushButton* pause = NULL;
    pausescreen* pausepage = NULL;
    QPushButton* help = NULL;
    QFile archives[3];
    QDialog* load_archive = NULL;
    QPushButton* archivebtn = NULL;
    QDialog* save_archive = NULL;
    QPushButton* savebtn = NULL;
    endpage* end = NULL;
    bool removing[2] = { false };
    int removingtime = 50;
    int line[2] = { 0 };
    int cnt[2] = { 0 };
    int score[2] = { 0 };
    bool hinting = false;
    int hintcnt = 0;
    int hinttime = 10 * 100;
    bool freezing[2] = { false };
    int freezecnt[2] = { 0 };
    int freezetime = 3 * 100;
    bool dizzy[2] = { false };
    int dizzycnt[2] = { 0 };
    int dizzytime = 10 * 100;
    box* solution1 = NULL;
    box* solution2 = NULL;//
    int speed = 3;
    bool Press_A = false;
    bool Press_S = false;
    bool Press_W = false;
    bool Press_D = false;
    bool Press_UP = false;
    bool Press_DOWN = false;
    bool Press_LEFT = false;
    bool Press_RIGHT = false;
    QPoint point1[2] = { {0,0} };
    QPoint point2[2] = { {0,0} };
    QPoint point3[2] = { {0,0} };
    QPoint point4[2] = { {0,0} };

    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    bool link(const box* a, const box* b, int cha);
    bool link1(QPoint a, QPoint b, int cha);
    bool link2(QPoint a, QPoint b, int cha);
    bool link3(QPoint a, QPoint b, int cha);
    bool findsol();
    void judge(box* a, int cha);
    void game_end();
    bool collide(QPoint x, QPoint y, int r);
    void move(QPoint p, int cha);
    void keyboarddetect();
    void use_tool(int cha);

private:
    Ui::twoplayer* ui;
};

#endif // TWOPLAYER_H
