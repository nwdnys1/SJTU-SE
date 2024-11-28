#pragma once

#include <cassert>
#include <iostream>

class Stack {
    // TODO
 private:
    int topp;
    int maxsize;//元素容量
    void doublespace(){
        int *tmp =array;
        array=new int[2*maxsize];
        for (int i = 0; i < maxsize; ++i) {
            array[i]=tmp[i];
        }
        maxsize*=2;
        delete []tmp;
    }

 public:
    int *array;
    Stack() {
        // TODO
        array=new int[5];
        maxsize = 5;
        topp=-1;
        for (int i=0;i<5;i++){
            array[i]=0;
        } //初始化
    }

    
    ~Stack() {
        // TODO
        //delete []array;
    }
    
    int size() {
        // TODO
        return topp+1;
    }
    
    bool empty() {
        return size() == 0;
    }
    
    void push(int t) {
        // TODO
        if(topp==maxsize-1) doublespace();
        array[++topp]=t;
    }
    
    void pop() {
        // TODO
        array[topp]=0;
        topp--;
    }
    
    int & top() const {
        // TODO
        return array[topp];
    }

    void show(){
        for (int i = 0; i < size(); ++i) {
            std::cout<<array[i]<<std::endl;
        }
    }
};
