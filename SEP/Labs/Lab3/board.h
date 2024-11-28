#pragma once
#include "stack.h"
class Board {
    // TODO
private:
    Stack rod[3];
    int disk;
    Stack fr;
    Stack to;
public:
    Board(int num_disk);
    ~Board();
    void draw();
    void move(int from, int to, bool log = true);
    bool win();
    void autoplay();
    void hanoi(int n,int a ,int b,int c);
    bool legal(int f,int t){
        if(f<=3&&f>=1&&t<=3&&t>=1&&t!=f){
            if(!rod[f-1].empty()){
                if(rod[t-1].empty()){
                    return true;
                }
                else{
                    if(rod[f-1].top()<rod[t-1].top()){
                        return true;
                    }
                }
            }
        }
        return false;
    }
};
