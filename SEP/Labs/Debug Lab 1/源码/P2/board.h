#pragma once

#include <utility>

#include "disk.h"
#include "rod.h"
#include "stack.h"
#include "canvas.h"

using namespace std;

class Board {
    const int num_disk;
    Rod *rods[3];
    const Disk **disks;
    Stack<pair<int, int>> history;

public:
    Board(int num_disk);
    ~Board();
    void draw();
    void move(int from, int to, bool log = true);
    bool win();
    void autoplay();
};
