#pragma once

#include <cassert>

#include "disk.h"
#include "stack.h"
#include "canvas.h"

class Rod {
    int capacity;
    int id;

 public:
    Stack<const Disk *> *stack;

    Rod(int capacity, int id) : capacity(capacity), id(id) {
        stack = new Stack<const Disk *>();
    }

    ~Rod() { delete stack; }

    bool push(const Disk *d) {
        if (stack->size() == capacity) return false;
        stack->push(d);
        return true;
    }

    const Disk *top() const {
        assert(stack->size() > 0 && stack->size() <= capacity);
        return stack->top();
    }

    void pop() { stack->pop(); }

    int size() const { return stack->size(); }

    bool empty() const { return stack->empty(); }

    bool full() const { return stack->size() == capacity; }

    void draw(Canvas& canvas) const {
        if (stack->size() == 0) return;

        Stack<const Disk*> tmp;
        int i = stack->size() - 1;

        while (!stack->empty()) {
            stack->top()->draw(canvas, i, id);
            tmp.push(stack->top());
            stack->pop();
            i--;
        }

        while (!tmp.empty()) {
            stack->push(tmp.top());
            tmp.pop();
        }
    }
};
