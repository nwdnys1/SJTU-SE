#pragma once
#ifndef PAUSESCREEN_H
#define PAUSESCREEN_H

#include <QDialog>

namespace Ui {
    class pausescreen;
}

class pausescreen : public QDialog
{
    Q_OBJECT

public:
    explicit pausescreen(QWidget* parent = nullptr);
    ~pausescreen();

    QPushButton* save = NULL;
    QPushButton* load = NULL;
    QPushButton* resume = NULL;
    QPushButton* quit = NULL;

private:
    Ui::pausescreen* ui;
};

#endif // PAUSESCREEN_H
