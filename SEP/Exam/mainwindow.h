#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QWidget>
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QBrush>
#include <QPushButton>
#include <QPen>
#include <QFileDialog>
#include <QColor>
#include <QPainter>
#include <QTextStream>
#include<queue>
#include <QLabel>
#include <QTime>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    vector<vector<int>> field;
    vector<vector<int>> safe;
    vector<QPoint> fire;
    vector<vector<QLabel>> text;
    int w;
    int h;
    int size=60;
    int minsafe=0;
    bool showsafe=false;
    QColor fire_color=Qt::red;
    QColor safe_color=Qt::green;
    QColor path_color=Qt::blue;
    QPushButton load;

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void transferMatrix();
    bool findPath();
    void showResult();
};

#endif // MAINWINDOW_H
