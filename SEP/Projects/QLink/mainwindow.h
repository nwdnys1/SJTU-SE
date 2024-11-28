#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <singleplayer.h>
#include <twoplayer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    single* game1 = NULL;
    twoplayer* game2 = NULL;
    int w = 1200;
    int h = 900;

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
