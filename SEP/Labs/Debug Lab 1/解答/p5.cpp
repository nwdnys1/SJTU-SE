#include "stack.h"
#include <iostream>

using namespace std;

Stack::Stack(int size){
        this->size = size;
        this->index = 0;
        ptr = new int[size];
}

Stack::Stack(const Stack &s){
        size = s.size;
        this->index = 0;
        ptr = new int[size];
        for (int i = 0; i < size; i++){
                ptr[i] = s.ptr[i];
        }
}

Stack::Stack(Stack &&s){
        size = s.size;
        this->index = 0;
        ptr = s.ptr;
        // delete [] a nullptr is safe
        s.ptr = nullptr;
}

Stack::~Stack(){
        //main函数中是直接构造的stack 不是new出来的 所以不需要手动delete 如果delete了在程序最后会报错 这是第一点；第二 如果delete了 会导致main函数中的stack_arr[i] = Stack(20);出错 因为Stack(20)是右值 右值是临时变量 在表达式结束时会调用析构函数 也就是被delete了 而在=的重载函数中 把数组的指针直接赋值了 导致左边栈的指针指向了一个被删除的内存区域 也就导致了错误
}

Stack &Stack::operator=(const Stack &s){
        if (this == &s)
                return *this;

        ptr = s.ptr;
        size = s.size;
        index = 0;

        return *this;
}

Stack &Stack::operator=(Stack &&s){
        *this = s;

        return *this;
}

void Stack::push(int val){
        if (index < size)
                ptr[index++] = val;
}

int Stack::pop(){
        if (!index)
                return -1;

        return ptr[--index];
}

int Stack::top(){
        if (!index)
                return -1;

        return ptr[index - 1];
}

bool Stack::empty(){
        return !index;
}