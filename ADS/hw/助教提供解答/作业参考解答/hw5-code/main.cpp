#include <iostream>
#include "rbtree.h"
int main() {
    RedBlackTree tree;

    tree.insert(7);
    tree.insert(3);
    tree.insert(18);
    tree.insert(10);
    tree.insert(22);
    tree.insert(8);
    tree.insert(11);
    tree.insert(26);

    std::cout << "Inorder traversal of the constructed tree: \n";
    tree.inorder();
    std::cout << std::endl;
    tree.preorder();
    std::cout << std::endl;

    return 0;
}
