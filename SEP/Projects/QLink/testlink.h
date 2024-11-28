#pragma once
#ifndef TESTLINK_H
#define TESTLINK_H

#include <QObject>
#include <QtTest/QtTest>
#include <singleplayer.h>
class testlink : public QObject
{
    Q_OBJECT
public:
    testlink();
    single* test;

private slots:
    void link_data();
    void link();

};

#endif // TESTLINK_H




