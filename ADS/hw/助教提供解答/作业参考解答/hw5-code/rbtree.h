#ifndef RBTREE_H
#define RBTREE_H

enum Color { RED, BLACK };

struct Node {
    int data;
    Color color;
    Node *parent, *left, *right;

    Node(int data);
};

struct fixInfo {
	int color_cnt;
	int rotate_cnt;
};

class RedBlackTree {
private:
    Node* root;

    void rotateLeft(Node* x);
    void rotateRight(Node* x);
    fixInfo fixViolation(Node* pt);
    Node* BSTInsert(Node* root, Node* pt);
    void inorderUtil(Node* root);
	void preorderUtil(Node *root);

public:
    RedBlackTree();
	fixInfo insert(const int data);
    void inorder();
	void preorder();
};

#endif // RBTREE_H
