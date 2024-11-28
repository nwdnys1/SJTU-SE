#include <iostream>

class Stack{
        int *ptr;
        int size;
        int index;

public:
        Stack(int size = 10);
        Stack(const Stack &s);
        Stack(Stack &&s);
        ~Stack();
        Stack &operator=(const Stack &s);
        Stack &operator=(Stack &&s);
        void push(int val);
        int pop();
        int top();
        bool empty();
};