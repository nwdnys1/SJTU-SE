#ifndef RBTREE_H
#define RBTREE_H

enum Color
{
    RED,
    BLACK
};

struct Node
{
    int data;
    Color color;
    Node *parent, *left, *right;

    Node(int data);
};

class RedBlackTree
{
private:
    Node *root;
    void rotateLeft(Node *x);
    void rotateRight(Node *x);
    void fixViolation(Node *pt);
    Node *BSTInsert(Node *root, Node *pt);
    void inorderUtil(Node *rootz);

public:
    int fixTime = 0;
    int rotateTime = 0;
    int colorTime = 0;
    RedBlackTree();
    void insert(const int data);
    void inorder();
};

#endif // RBTREE_H
