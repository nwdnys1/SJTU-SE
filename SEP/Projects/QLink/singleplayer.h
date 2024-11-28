#pragma once
#ifndef SINGLE_H
#define SINGLE_H

#include <QWidget>
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
    class single;
}

class single : public QWidget
{
    Q_OBJECT
public:
    explicit single(QWidget* parent = nullptr);
    ~single();

    map* m = NULL;
    int pixwidth = 1600;
    int pixheight = 900;
    int mapx;
    int mapy;
    QPoint mappos;
    QRect boundary;
    character* A = NULL;
    int activated = 0;//表示当前激活个数
    box* chosen = NULL;
    box* chosen2 = NULL;
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
    bool removing = false;
    int removetime = 50;
    int line = 0;
    int cnt = 0;
    int score = 0;
    bool hinting = false;
    int hintcnt = 0;
    int hinttime = 10 * 100;
    bool flashing = false;
    int flashcnt = 0;
    int flashtime = 5 * 100;
    box* solution1 = NULL;
    box* solution2 = NULL;
    int speed = 3;
    QPoint point1 = { 0,0 };
    QPoint point2 = { 0,0 };
    QPoint point3 = { 0,0 };
    QPoint point4 = { 0,0 };
    bool Press_A = false;
    bool Press_S = false;
    bool Press_W = false;
    bool Press_D = false;

    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool link(const box* a, const box* b);
    bool link1(QPoint a, QPoint b);
    bool link2(QPoint a, QPoint b);
    bool link3(QPoint a, QPoint b);
    bool findsol();
    void judge(box* a);
    void game_end();
    bool collide(QPoint x, QPoint y, int r);
    void move(QPoint p);
    void keyboarddetect();
    void flashto(QPoint p);
    void use_tool();

private:
    Ui::single* ui;
};

#endif // SINGLE_H
