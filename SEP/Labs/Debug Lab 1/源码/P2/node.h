#pragma once

template <typename T>
struct Node {
    T val;
    Node<T> *next;
    Node() : next(nullptr) {}
    Node(T t) : val(t), next(nullptr) {}
};
