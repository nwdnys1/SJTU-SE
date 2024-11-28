#pragma once
#ifndef ENDPAGE_H
#define ENDPAGE_H

#include <QDialog>
#include<QLabel>

namespace Ui {
    class endpage;
}

class endpage : public QDialog
{
    Q_OBJECT

public:
    explicit endpage(QWidget* parent = nullptr);
    ~endpage();

    QPushButton* return_menu = NULL;
    QLabel* text = NULL;

private:
    Ui::endpage* ui;
};

#endif // ENDPAGE_H
