#include "mainwindow.h"

#include <iostream>
#include <string>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(1200, 900);
    this->setWindowTitle("Find Path!");
    load.setParent(this);
    load.setGeometry(1000,50,100,50);
    load.setText("LOAD");
    connect(&load, &QPushButton::clicked, [=] {//加载按钮
        QFileDialog d;
        QString filename=d.getOpenFileName(this,"加载文件","D:/OneDrive - sjtu.edu.cn/COLLEGE/SEP/SEP2023","txt文件(*.txt)");
        if(filename=="") return;
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            in>>w>>h;
            field=vector<vector<int>>(w,vector<int>(h,0));
            safe=vector<vector<int>>(w,vector<int>(h,INT_MAX));
            showsafe=0;
            fire.clear();
            for (int j = 0; j < h; ++j) {
                for (int i=0; i<w ;++i) {
                    in>>field[i][j];
                    if(field[i][j]) fire.push_back({i,j});
                }
            }
            in>>minsafe;
            file.close();
        }
    });
    // @TODO: write your code here
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    // @TODO: write your code here
    if(event->key()==Qt::Key_T) transferMatrix();
    if(event->key()==Qt::Key_P) showResult();
}

void MainWindow::showResult(){
    // @TODO: write your code here
    if(field.empty()) {
        QDialog d;
        QLabel l(&d);
        d.setWindowTitle("error");
        l.setText("还未载入文件");
        l.setAlignment(Qt::AlignCenter);
        d.exec();
        return;
    }
    if(!showsafe) {
        QDialog d;
        QLabel l(&d);
        d.setWindowTitle("error");
        l.setText("还未计算安全程度");
        l.setAlignment(Qt::AlignCenter);
        d.exec();
        return;
    }
    if(!findPath()){
        QDialog d;
        QLabel l(&d);
        d.setWindowTitle("error");
        l.setText("can not find path.");
        l.setAlignment(Qt::AlignCenter);
        d.exec();
    }
}

void MainWindow::transferMatrix(){
    // @TODO: write your code here, subproblem one
    // You can add parameters to this method as needed
    for (int i = 0; i < w; ++i) {
        for (int j=0; j<h ;++j) {
            if(field[i][j]==1) safe[i][j]=0;
            else if(fire.empty()) safe[i][j]=0;
            else for(auto it:fire) safe[i][j]=min(abs(i-it.x())+abs(j-it.y()),safe[i][j]);
        }
    }
    showsafe=1;
    update();
}

bool MainWindow::findPath(){
    // @TODO: write your code here, subproblem two
    // You can add parameters to this method as needed

    vector<QPoint> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    queue<QPoint> q;
    vector<QPoint> path;
    vector<vector<int>> vis(w,vector<int>(h,0));
    QPoint pre[w][h];
    if(safe[0][0]<minsafe) return false;
    q.push({0,0});
    vis[0][0] = 1;
    while(!q.empty()) {
        QPoint tmp = q.front();
        q.pop();
        int x = tmp.x();
        int y = tmp.y();
        if(x == w - 1 && y == h - 1) {
            QPoint end={x,y};
            while(true){
                field[end.x()][end.y()]=2;
                path.push_back(end);
                if(end.x()==0&&end.y()==0) {
                    field[0][0]=2;
                    break;
                }
                end = pre[end.x()][end.y()];
            }
            for (int j=0;j<h;j++) {
                for (int i=0;i<w;i++) {
                    cout<<safe[i][j]<<" ";
                }
                cout<<endl;
            }
            for(int i=path.size()-1;i>=0;i--){
                cout<<path[i].y()<<" "<<path[i].x()<<endl;
            }
            update();
            return true;
        }
        for(auto dir : directions) {
            int i = x + dir.x();
            int j = y + dir.y();
            if(i >= 0 && i <w && j >= 0 && j < h && vis[i][j] == 0 &&field[i][j]!=1 && safe[i][j]>=minsafe) {
                vis[i][j] = 1;
                q.push({i, j});
                pre[i][j]=tmp;
            }
        }
    }

    return false;
}

void MainWindow::paintEvent(QPaintEvent *event){
    // @TODO: write your code here
    QPainter painter(this);
    for (int i = 0; i < w; ++i) {
        for (int j=0; j<h ;++j) {
            QRect rec(i*size,j*size,size,size);
            if (field[i][j]==0) painter.fillRect(rec,safe_color);
            else if(field[i][j]==1) painter.fillRect(rec,fire_color);
            else if(field[i][j]==2) painter.fillRect(rec,path_color);
            painter.drawRect(rec);
            rec.setX(i*size+size/2);rec.setY(j*size+size/2);
            if(showsafe) painter.drawText(rec,QString::number(safe[i][j]));

        }
    }
}
