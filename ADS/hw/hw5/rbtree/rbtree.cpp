#include "rbtree.h"
#include <iostream>

// The color of new node is always red
Node::Node(int data) : data(data), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}

RedBlackTree::RedBlackTree() : root(nullptr) {}

void RedBlackTree::rotateLeft(Node *x)
{
    // Left rotation logic
    rotateTime++;
    Node *y = x->right;
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

void RedBlackTree::rotateRight(Node *x)
{
    // Right rotation logic
    rotateTime++;
    Node *y = x->left;
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

void RedBlackTree::fixViolation(Node *pt)
{
    // Fix violation logic
    
    Node *parent_pt = nullptr;
    Node *grand_parent_pt = nullptr;
    if((pt != root) && (pt->color == RED) && (pt->parent->color == RED)) fixTime++;
    while ((pt != root) && (pt->color == RED) && (pt->parent->color == RED))
    {   
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        if (parent_pt == grand_parent_pt->left)
        {
            Node *uncle_pt = grand_parent_pt->right;

            // Uncle is red
            if (uncle_pt != nullptr && uncle_pt->color == RED)
            {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                colorTime += 3;
                pt = grand_parent_pt;
            }
            else
            { // Uncle is black
                // LL-Case and LR-Case, write your code here
                if (pt == parent_pt->left)
                {
                    rotateRight(grand_parent_pt);
                    std::swap(parent_pt->color, grand_parent_pt->color);
                    colorTime += 2;
                    pt = parent_pt;
                }
                else
                {
                    rotateLeft(parent_pt);
                    pt = parent_pt;
                }
            }
        }
        else
        {
            Node *uncle_pt = grand_parent_pt->left;

            if ((uncle_pt != nullptr) && (uncle_pt->color == RED))
            {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                colorTime += 3;
                pt = grand_parent_pt;
            }
            else
            {
                // RR-Case and RL-Case, write your code here
                if (pt == parent_pt->right)
                {
                    rotateLeft(grand_parent_pt);
                    std::swap(parent_pt->color, grand_parent_pt->color);
                    colorTime += 2;
                    pt = parent_pt;
                }
                else
                {
                    rotateRight(parent_pt);
                    pt = parent_pt;
                }
            }
        }
    }

    root->color = BLACK;

}

Node *RedBlackTree::BSTInsert(Node *root, Node *pt)
{
    // Binary search tree insert logic
    if (root == nullptr)
        return pt;

    if (pt->data < root->data)
    {
        root->left = BSTInsert(root->left, pt);
        root->left->parent = root;
    }
    else if (pt->data > root->data)
    {
        root->right = BSTInsert(root->right, pt);
        root->right->parent = root;
    }

    return root;
}

void RedBlackTree::inorderUtil(Node *root)
{
    // Inorder traversal logic
    if (root == nullptr)
        return;
    inorderUtil(root->left);
    std::cout << root->data << " (" << (root->color == RED ? "RED" : "BLACK") << ") ";
    inorderUtil(root->right);
}

void RedBlackTree::insert(const int data)
{
    // Insertion logic
    Node *new_node = new Node(data);
    root = BSTInsert(root, new_node);
    fixViolation(new_node);
}

void RedBlackTree::inorder()
{
    inorderUtil(root);
}
