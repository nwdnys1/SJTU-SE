#pragma once

#include <cassert>

#include "node.h"

template <typename T> class Stack {
    int sz;
    Node<T> *head;

 public:
    Stack() : sz(0), head(nullptr) {}

    ~Stack() {
        while (head != nullptr) {
            Node<T> *tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    void push(T t) {
        sz++;
        Node<T> *node = new Node<T>(t);
        node->next = head;
        head = node;
    }

    void pop() {
        assert(size() > 0);

        if (head != nullptr) {
            sz--;
            Node<T> *tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    T& top() {
        assert(size() > 0);
        return head->val;
    }

    bool empty() {
        return sz == 0;
    }

    int size() {
        return sz;
    }
};
