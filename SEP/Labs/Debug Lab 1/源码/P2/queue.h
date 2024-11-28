#pragma once

#include <cassert>

#include "node.h"

template <typename T> class Queue {
    int sz;
    Node<T> *head;
    Node<T> *tail;

 public:
    Queue() : sz(0), head(nullptr), tail(nullptr) {}

    ~Queue() {
        while (head != nullptr) {
            Node<T> *tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    int size() const {
        return sz;
    }

    void push(T t) {
        Node<T> *tmp = new Node<T>(t);
        if (sz == 0) {
            head = tmp;
        } else {
            tail->next = tmp;
        }
        tail = tmp;
        sz++;
    }

    void pop() {
        if (sz > 0) {
            Node<T> *tmp = head;
            head = head->next;
            delete tmp;
            sz--;
        }
    }

    T& front() {
        assert(head != nullptr);
        return head->val;
    }

    bool empty() {
        return sz == 0;
    }
};