#include <iostream>

#include "queue.h"
#include "board.h"

Board::Board(int num_disk) : num_disk(num_disk) {
    for (int i = 0; i < 3; i++) {
        rods[i] = new Rod(num_disk, i);
    }

    for (int i = 0; i < num_disk; i++) {
        disks[i] = new Disk(num_disk - i - 1, 2 * num_disk + 1 - 2 * i);
        rods[0]->push(disks[i]);
    }
}

Board::~Board() {
    for (int i = 0; i < 3; i++)
        delete rods[i];
    for (int i = 0; i < num_disk; i++)
        delete disks[i];

    delete[] disks;
}

void Board::draw() {
    Canvas canvas;
    canvas.reset();
    for (int i = 0; i < Canvas::WIDTH; i++) {
        canvas.buffer[Canvas::HEIGHT - 1][i] = '-';
    }
    for (int i = 0; i < Canvas::HEIGHT; i++) {
        canvas.buffer[i][5] = '|';
        canvas.buffer[i][20] = '|';
        canvas.buffer[i][35] = '|';
    }
    for (int i = 0; i < 3; i++) {
        rods[i]->draw(canvas);
    }
    canvas.draw();
}

void Board::move(int from, int to, bool log) {
    --from;
    --to;

    if (from < 0 || from > 2 || to < 0 || to > 2)
        return;
    if (rods[from]->top()->val > rods[to]->top()->val)
        return;
    if (rods[to]->full())
        return;

    rods[to]->push(rods[from]->top());
    rods[from]->pop();

    if (log) {
        history.push(make_pair(from, to));
    }
}

bool Board::win() {
    return rods[1]->size() == num_disk;
}

void solve(int n, int src, int buf, int dest, Queue<pair<int,int>> solution) {
    if (n >= 1) {
        solve(n - 1, src, dest, buf, solution);
        solution.push(make_pair(src, dest));
        solve(n - 1, buf, src, dest, solution);
    }
}

void Board::autoplay() {
    while (!history.empty()) {
        auto p = history.top();
        history.pop();
        cout << "Auto moving:" << p.second + 1 << "->" << p.first + 1 << endl;
        move(p.second + 1, p.first + 1, false);
        draw();
    }

    Queue<pair<int,int>> solution;
    solve(num_disk, 0, 2, 1, solution);
    while (!solution.empty()) {
        pair<int,int> step = solution.front();
        int from = step.first + 1, to = step.second + 1;

        cout << "Auto moving:" << from << "->" << to << endl;
        move(from, to);
        draw();

        solution.pop();
    }
}

