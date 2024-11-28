#include "rbtree.h"
#include <iostream>

// The color of new node is always red
Node::Node(int data) : data(data), color(RED), parent(nullptr), left(nullptr), right(nullptr) {} 

RedBlackTree::RedBlackTree() : root(nullptr) {}

void RedBlackTree::rotateLeft(Node* x) {
    // Left rotation logic
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void RedBlackTree::rotateRight(Node* x) {
    // Right rotation logic
    Node* y = x->left;
    x->left = y->right;
    if (y->right != nullptr)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

fixInfo RedBlackTree::fixViolation(Node* pt) {
    // Fix violation logic
    Node* parent_pt = nullptr;
    Node* grand_parent_pt = nullptr;

	int color_cnt = 0, rotate_cnt = 0;
    
    while ((pt != root) && (pt->color == RED) && (pt->parent->color == RED)) {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        if (parent_pt == grand_parent_pt->left) {
            Node* uncle_pt = grand_parent_pt->right;

            // Uncle is red
            if (uncle_pt != nullptr && uncle_pt->color == RED) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
				color_cnt += 3;
            } else {// Uncle is black
				if (pt == parent_pt->left) { // LL
					grand_parent_pt->color = RED;
					parent_pt->color = BLACK;
					rotateRight(grand_parent_pt);
					color_cnt += 2;
					rotate_cnt += 1;
				} else { // LR
					grand_parent_pt->color = RED;
					pt->color = BLACK;
					rotateLeft(parent_pt);
					rotateRight(grand_parent_pt);
					color_cnt += 2;
					rotate_cnt += 2;
				}
            }
        } else {
            Node* uncle_pt = grand_parent_pt->left;

            if ((uncle_pt != nullptr) && (uncle_pt->color == RED)) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
				color_cnt += 3;
            } else {
				if (pt == parent_pt->right) { // RR
					grand_parent_pt->color = RED;
					parent_pt->color = BLACK;
					rotateLeft(grand_parent_pt);
					color_cnt += 2;
					rotate_cnt += 1;
				} else { // RL
					grand_parent_pt->color = RED;
					pt->color = BLACK;
					rotateRight(parent_pt);
					rotateLeft(grand_parent_pt);
					color_cnt += 2;
					rotate_cnt += 2;
				}
            }
        }
    }

	if (root->color != BLACK) {
		root->color = BLACK;
		++color_cnt;
	}
	return {color_cnt, rotate_cnt};
}

Node* RedBlackTree::BSTInsert(Node* root, Node* pt) {
    // Binary search tree insert logic
    if (root == nullptr)
        return pt;

    if (pt->data < root->data) {
        root->left = BSTInsert(root->left, pt);
        root->left->parent = root;
    } else if (pt->data > root->data) {
        root->right = BSTInsert(root->right, pt);
        root->right->parent = root;
    }

    return root;
}

void RedBlackTree::inorderUtil(Node* root) {
    // Inorder traversal logic
    if (root == nullptr)
        return;
    inorderUtil(root->left);
    std::cout << root->data << (root->color == RED ? "(R) " : "(B) ");
    inorderUtil(root->right);
}

void RedBlackTree::preorderUtil(Node* root) {
    // Inorder traversal logic
    if (root == nullptr)
        return;
    std::cout << root->data << (root->color == RED ? "(R) " : "(B) ");
    preorderUtil(root->left);
    preorderUtil(root->right);
}

fixInfo RedBlackTree::insert(const int data) {
    // Insertion logic
    Node* new_node = new Node(data);
    root = BSTInsert(root, new_node);
    return fixViolation(new_node);

    
}

void RedBlackTree::inorder() {
    inorderUtil(root);
}

void RedBlackTree::preorder() {
	preorderUtil(root);
}
