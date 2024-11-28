//
// Created by Smile on 2019/4/8.
//

#ifndef C_BINARYDIMEN_TREE_H
#define C_BINARYDIMEN_TREE_H

#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

/****************************************************************
 *                    Write your code below
 ****************************************************************/

struct point{
    long long x;
    long long y;
};

class TreeNode
{
  friend ostream &operator<<(ostream &out, const TreeNode &b);
  friend class BinaryTree;
  friend class BinaryDimonTree;

private:
  /* data */
  long long data[2];
  TreeNode* left;
  TreeNode* right;
  long long k;//
public:
  /* methods */
  TreeNode();
  long long getX();  /* DO NOT CHANGE */
  long long getY();  /* DO NOT CHANGE */
  ~TreeNode(); /* DO NOT CHANGE */
};


class BinaryDimonTree
{
friend istream &operator>>(istream &in, BinaryDimonTree &tree); /* DO NOT CHANGE */

private:
  /* data */
  TreeNode *root;

public:
  /* methods */
  BinaryDimonTree();          /* DO NOT CHANGE */
  TreeNode *find_nearest_node(long long x, long long y);  /* DO NOT CHANGE */

  void recur_search(TreeNode *cur, long long x, long long y, long long int &min_distance, TreeNode **guess);
  ~BinaryDimonTree();
  void clear(TreeNode *tmp);
  void buildBDTree(TreeNode *node, point *a, long long size, long long k);
};

#endif //C_BINARYDIMEN_TREE_H
