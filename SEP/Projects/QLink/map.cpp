#include "map.h"
#include "ui_map.h"
#include<QImage>
#include <QPainter>
#include <QQueue>
#include <QDebug>


map::map(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::map)
{
    ui->setupUi(this);
    //这个类是游戏地图类，主要包含了地图方块、道具以及几个函数，下面的代码是加载图片资源以及对道具数组初始化；
    this->setFixedSize(width * boxsize + 1, height * boxsize + 1);
    bool ld = img[1].load(":/new/prefix1/ft1.png");
    ld = img[2].load(":/new/prefix1/ft2.png");
    ld = img[3].load(":/new/prefix1/ac1.png");
    ld = img[4].load(":/new/prefix1/ac2.png");
    ld = img[5].load(":/new/prefix1/kn1.png");
    ld = img[6].load(":/new/prefix1/kn2.png");
    ld = img[7].load(":/new/prefix1/mg1.png");
    ld = img[8].load(":/new/prefix1/mg2.png");
    boxes = new box[width * height];
    pl = new plus1s[plus1s_num];
    sf = new shuffle[shuffle_num];
    ht = new hint[hint_num];
    fl = new flash[flash_num];
    fr = new freeze[freeze_num];
    dz = new diz[diz_num];
}

void map::paintEvent(QPaintEvent* event) {//在这里进行地图方块的绘制
    QPainter painter(this);
    for (int i = 0; i < width * height; ++i) {
        if (boxes[i].type) {
            QRect rec(boxes[i].pos * boxsize + QPoint(1, 1), boxes[i].pos * boxsize + QPoint(boxsize - 2, boxsize - 2));
            painter.drawImage(rec, img[boxes[i].type]);
        }
    }
}

void map::mapshuffle() {//用于地图的刷新，包括所有道具与角色位置也刷新；
    qsrand((unsigned)time(0));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            boxes[y * width + x].type = 0;
            boxes[y * width + x].pos = QPoint(x, y);
        }
    }
    for (int x = 1; x < width - 1; ++x) {
        for (int y = 1; y < height - 1; ++y) {
            boxes[y * width + x].type = qrand() % 9;
        }
    }
    for (int i = 0; i < plus1s_num; ++i) {
        pl[i].pos = randompos(pl[i].size);
    }
    for (int i = 0; i < shuffle_num; ++i) {
        sf[i].pos = randompos(sf[i].size);
    }
    for (int i = 0; i < hint_num; ++i) {
        ht[i].pos = randompos(ht[i].size);
    }
    for (int i = 0; i < flash_num; ++i) {
        fl[i].pos = randompos(fl[i].size);
    }
    for (int i = 0; i < freeze_num; ++i) {
        fr[i].pos = randompos(fr[i].size);
    }
    for (int i = 0; i < diz_num; ++i) {
        dz[i].pos = randompos(dz[i].size);
    }

}

QPoint map::randompos(int r) {//随机生成一个位置，要求不能生成在方块上，且与外界可以连通，输入参数为一个半径使其不会有一部分生成在方块内部，返回的是一个qpoint代表位置；
    QPoint p = { qrand() % (width * boxsize / 10) * 10,qrand() % (height * boxsize / 10) * 10 };
    QPoint a = p + QPoint(0, r);
    QPoint b = p + QPoint(0, -r);
    QPoint c = p + QPoint(r, 0);
    QPoint d = p + QPoint(-r, 0);
    QRect bdry(0, 0, width * boxsize, height * boxsize);
    while (!connectable(0, p.y() / boxsize * width + p.x() / boxsize) || in_box(a) || in_box(b) || in_box(c) || in_box(d) || !bdry.contains(a) || !bdry.contains(b) || !bdry.contains(c) || !bdry.contains(d)) {
        p = { qrand() % (width * boxsize / 10) * 10,qrand() % (height * boxsize / 10) * 10 };
        a = p + QPoint(0, r);
        b = p + QPoint(0, -r);
        c = p + QPoint(r, 0);
        d = p + QPoint(-r, 0);
    }
    return p;
}

int map::in_box(const QPoint& p) {//判断某一个点是否存在方块内部，参数为一个qpoint，返回一个int变量，若不存在方块返回0，若存在则返回其在boxes数组中的序号，这是为了方便在其他函数中的使用，可以直接找到这个点所在的方块；
    if (boxes[p.y() / boxsize * width + p.x() / boxsize].type) return p.y() / boxsize * width + p.x() / boxsize;
    else return 0;
}

bool map::connectable(int a, int b) {//判断两个方块是否连通，输入的是两个int代表方块在boxes数组中的序号，返回的是bool值，此函数主要用来实现flash时判断鼠标点击位置是否与角色当前位置连通，以及生成随机位置时防止生成在封闭内部；
    if (b / width == 0 || b / width == height - 1 || b % width == 0 || b % width == width - 1) return 1;
    QQueue<int> q;
    int* visited = new int[width * height];
    for (int i = 0; i < width * height; ++i) {
        visited[i] = 0;
    }
    q.enqueue(b);
    while (!q.isEmpty()) {
        int tmp = q.dequeue();
        if (a == tmp) return 1;
        if (tmp % width < width - 1 && !boxes[tmp + 1].type && !(visited[tmp + 1]++)) {
            q.enqueue(tmp + 1);
        }
        if (tmp % width > 0 && !boxes[tmp - 1].type && !(visited[tmp - 1]++)) {
            q.enqueue(tmp - 1);
        }
        if ((tmp + width) / width < height && !boxes[tmp + width].type && !(visited[tmp + width]++)) {
            q.enqueue(tmp + width);
        }
        if (tmp - width >= 0 && !boxes[tmp - width].type && !(visited[tmp - width]++)) {
            q.enqueue(tmp - width);
        }
    }
    return 0;
}

map::~map()
{
    delete ui;
}
