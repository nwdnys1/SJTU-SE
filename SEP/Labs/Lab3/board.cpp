#include "board.h"
#include "canvas.h"

Board::Board(int num_disk) {
    // TODO
    disk=num_disk;
    for(int i=0;i<=disk-1;i++){
        rod[0].push(2*(disk-1-i)+3);
    }
    /*for(int i=0;i<disk;i++){
        cout<<rod[0].array[i]<<endl;
    }*/
}

Board::~Board() {
    // TODO
}

void Board::draw() {
    Canvas canvas;
    canvas.reset();
    for(int j=0;j<41;j++){
        canvas.buffer[10][j]='-';
    }
    for(int i=0;i<11;i++){
        canvas.buffer[i][5]='|';
        canvas.buffer[i][20]='|';
        canvas.buffer[i][35]='|';
    }
    for(int i=1;i<10;i=i+2){
        if (rod[0].array[4-(i-1)/2]!=0){
            int t=rod[0].array[4-(i-1)/2];
            for(int j=5-(t-1)/2;j<=5+(t-1)/2;j++){
                canvas.buffer[i][j]='*';
            }
        }
        if (rod[1].array[4-(i-1)/2]!=0){
            int t=rod[1].array[4-(i-1)/2];
            for(int j=20-(t-1)/2;j<=20+(t-1)/2;j++){
                canvas.buffer[i][j]='*';
            }
        }
        if (rod[2].array[4-(i-1)/2]!=0){
            int t=rod[2].array[4-(i-1)/2];
            for(int j=35-(t-1)/2;j<=35+(t-1)/2;j++){
                canvas.buffer[i][j]='*';
            }
        }
    }
    canvas.draw();
    // TODO
}

void Board::move(int f, int t, bool log) {
    // TODO
    if (log){
        fr.push(f);to.push(t);
        cout<<fr.top()<<to.top()<<endl;
        int tmp=rod[f-1].top();
        rod[f-1].pop();
        rod[t-1].push(tmp);
        draw();
    }
    else{
        int tmp=rod[f-1].top();
        rod[f-1].pop();
        rod[t-1].push(tmp);
        cout<<"Auto moving:"<<f<<"->"<<t<<endl;
        draw();
    }

}

bool Board::win() {
    // TODO
    return (rod[0].empty()&&rod[2].empty());
}

void Board::hanoi(int n,int a,int b,int c){
    if(n==1) move(a,b,0);
    else{
        hanoi(n-1,a,c,b);
        move(a,b,0);
        hanoi(n-1,c,b,a);
    }
}
void Board::autoplay() {
    // TODO
    int n=fr.size();
    for (int i = 0; i < n; ++i) {
        move(to.top(),fr.top(),0);
        to.pop();fr.pop();
    }
    hanoi(disk,1,2,3);
    cout<<"Congratulations! You win!"<<endl;

}
