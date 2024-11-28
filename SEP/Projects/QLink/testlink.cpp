#include "testlink.h"
#include <QtTest/QtTest>


testlink::testlink() {
    test = new single;
    test->m = new map;
    test->m->width = 6;
    test->m->height = 6;
    for (int x = 0; x < test->m->width; ++x) {
        for (int y = 0; y < test->m->height; ++y) {
            test->m->boxes[y * test->m->width + x].type = 0;
            test->m->boxes[y * test->m->width + x].pos = QPoint(x, y);
        }
    }
    test->m->boxes[1 * test->m->width + 1].type = 2;
    test->m->boxes[1 * test->m->width + 3].type = 1;
    test->m->boxes[2 * test->m->width + 1].type = 1;
    test->m->boxes[2 * test->m->width + 3].type = 1;
    test->m->boxes[3 * test->m->width + 1].type = 2;
    test->m->boxes[3 * test->m->width + 2].type = 1;
    test->m->boxes[3 * test->m->width + 3].type = 2;
    test->m->boxes[3 * test->m->width + 4].type = 1;
    test->m->boxes[4 * test->m->width + 3].type = 1;
    /*构造矩阵如下：
         0,0,0,0,0,0
         0,2,0,1,0,0
         0,1,0,1,0,0
         0,2,1,2,1,0
         0,0,0,1,0,0
         0,0,0,0,0,0  */
}

void testlink::link_data()
{

    QTest::addColumn<int>("box1");
    QTest::addColumn<int>("box2");
    QTest::addColumn<bool>("TorF");//是否能连接
    QTest::addColumn<int>("NumofLines");//连接需要的折线数
    //所有if判断的路径对应了以下8种情况：
    QTest::newRow("1") << 1 * test->m->width + 1 << 1 * test->m->width + 1 << false << 0;//同一个方块连接同一个方块；
    QTest::newRow("2") << 1 * test->m->width + 1 << 1 * test->m->width + 3 << false << 0;//不同类型的方块连接；
    QTest::newRow("3") << 2 * test->m->width + 1 << 2 * test->m->width + 3 << true << 1;//可以用一条折线连接；
    QTest::newRow("4") << 1 * test->m->width + 1 << 3 * test->m->width + 1 << true << 3;//不能用一条折线连接，但用三条可以；
    QTest::newRow("5") << 3 * test->m->width + 1 << 3 * test->m->width + 3 << false << 0;//既不能用一条也不能用三条折线连接；
    QTest::newRow("6") << 1 * test->m->width + 3 << 3 * test->m->width + 4 << true << 2;//可以用两条折线连接；
    QTest::newRow("7") << 2 * test->m->width + 1 << 1 * test->m->width + 3 << true << 3;//不能用两条折线连接，但能用三条；
    QTest::newRow("8") << 2 * test->m->width + 1 << 3 * test->m->width + 4 << false << 0;//既不能用两条折线也不能用三条连接；


}


void testlink::link()
{
    QFETCH(int, box1);
    QFETCH(int, box2);
    QFETCH(bool, TorF);
    QFETCH(int, NumofLines);

    QCOMPARE(test->link(&(test->m->boxes[box1]), &(test->m->boxes[box2])), TorF);
    QCOMPARE(test->line, NumofLines);
}

//QTEST_MAIN(testlink)


