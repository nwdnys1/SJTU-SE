#pragma once
#ifndef MAP_H
#define MAP_H

#include <QWidget>

namespace Ui {
    class map;
}
struct character {
    QPoint pos = { 0,0 };
    int size = 10;
};
struct box {
    QPoint pos = { 0,0 };
    int type = 0;//0为空 123..为类型
    int activated = 0;
};
struct plus1s {
    QPoint pos;
    int size = 5;
};
struct shuffle {
    QPoint pos;
    int size = 5;
};
struct hint {
    QPoint pos;
    int size = 5;
};
struct flash {
    QPoint pos;
    int size = 5;
};
struct freeze {
    QPoint pos;
    int size = 5;
};
struct diz {
    QPoint pos;
    int size = 5;
};

class map : public QWidget {
    Q_OBJECT

public:
    explicit map(QWidget* parent = nullptr);
    ~map();

    box* boxes;
    QImage* img = new QImage[20];
    int plus1s_num = 3;
    int shuffle_num = 3;
    int hint_num = 3;
    int flash_num = 3;
    int freeze_num = 3;
    int diz_num = 3;
    plus1s* pl = NULL;
    shuffle* sf = NULL;
    hint* ht = NULL;
    flash* fl = NULL;
    freeze* fr = NULL;
    diz* dz = NULL;
    int width = 12;
    int height = 8;//地图大小
    int boxsize = 100;

    void paintEvent(QPaintEvent* event) override;
    void mapshuffle();
    QPoint randompos(int r);
    int in_box(const QPoint& p);
    bool connectable(int a, int b);

private:
    Ui::map* ui;
};

#endif // MAP_H
