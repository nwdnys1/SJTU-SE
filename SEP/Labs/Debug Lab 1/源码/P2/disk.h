#pragma once

#include "canvas.h"

class Disk {
public:
    int id;
    int val;
    Disk(int id, int val) : id(id), val(val) {}

    void draw(Canvas& canvas, int level, int rod_loc) const {
        int s_x = 5 + (rod_loc * 15) - (val / 2);
        int y = Canvas::HEIGHT - (level + 1) * 2;
        for (int i = 0; i < val; i++) {
            canvas.buffer[y][s_x + i] = '*';
        }
    }
};
