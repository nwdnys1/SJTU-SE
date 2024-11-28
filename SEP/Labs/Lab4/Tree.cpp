#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <vector>
#include <queue>
#include <algorithm>

#include "Tree.h"

using namespace std;

/****************************************************************
 *                    Write your code below
 ****************************************************************/
ostream &operator<<(ostream &out, const TreeNode &b) {
    // TODO: Your code here
    out<<'('<<b.data[0]<<')'<<b.data[1];
    return out;
}

long long TreeNode::getX() {
    // TODO: Your code here
    return data[0];
}

long long TreeNode::getY() {
    // TODO: Your code here
    return data[1];
}

TreeNode::TreeNode() {
    data[0]=0;
    data[1]=0;
    left=NULL;
    right=NULL;
    k=0;
}
TreeNode::~TreeNode() {
    // TODO: Your code here
}

bool compareX(point a,point b)//用于sort的排序规则
{
    return a.x<b.x;
}

bool compareY(point a,point b)
{
    return a.y<b.y;
}

void BinaryDimonTree::buildBDTree(TreeNode* node,point a[],long long size,long long k)//递归实现输入2dtree
{
    if(size==0)
    {
        node->left=NULL;
        node->right=NULL;
        return;
    }
    if(k%2==0)
        sort(a,a+size,compareX);
    else
        sort(a,a+size,compareY);
    node->data[0]=a[size/2].x;
    node->data[1]=a[size/2].y;
    node->k=k;
    node->left=new TreeNode;
    node->right=new TreeNode;
    if(size/2!=0)
    {
        node->left=new TreeNode;
        buildBDTree(node->left,a,size/2,k+1);
    }
    else node->left=NULL;

    if(size-size/2-1!=0)
    {
        node->right=new TreeNode;
        buildBDTree(node->right,a+size/2+1,size-size/2-1,k+1);
    }
    else node->right=NULL;

}

istream &operator>>(istream &in, BinaryDimonTree &tree) {
    // TODO: Your code here
    long long Num,i;
    in>>Num;
    point p[Num];
    for(i=0;i<Num;i++)
    {in>>p[i].x>>p[i].y;}
    tree.buildBDTree(tree.root,p,Num,0);
    return in;
}

BinaryDimonTree::BinaryDimonTree() {
    // TODO: Your code here
    root=new TreeNode();
}

TreeNode *BinaryDimonTree::find_nearest_node(long long x, long long y) {
    // TODO: Your code here
    long long bestDist=10000000000;
    TreeNode* guess=NULL;
    recur_search(root,x,y,bestDist,&guess);
    return guess;
}

long long distance(TreeNode* x,long long a,long long b){//计算距离的平方
    return ((x->getX()-a)*(x->getX()-a)+(x->getY()-b)*(x->getY()-b));
}

void BinaryDimonTree::recur_search(TreeNode *cur, long long x, long long y, long long &min_distance, TreeNode **guess) {
    // TODO: Your code here
    if(cur==NULL) return;

    if(distance(cur,x,y)<min_distance){
        min_distance=distance(cur,x,y);
        *guess=cur;
    }

    if(distance(cur,x,y)==min_distance){
        if(cur->data[0]<(*guess)->data[0])
            *guess=cur;
        else
        if(cur->data[0]==(*guess)->data[0]&&cur->data[1]<(*guess)->data[1])
            *guess=cur;
    }
    long long c;
    if ((cur->k)%2==0) c=x;
    else c=y;

    if(c<cur->data[(cur->k)%2]){
        recur_search(cur->left,x,y,min_distance,guess);
        if((c-cur->data[(cur->k)%2])*(c-cur->data[(cur->k)%2])<min_distance)
            recur_search(cur->right,x,y,min_distance,guess);
    }
    else{
        recur_search(cur->right,x,y,min_distance,guess);
        if((c-cur->data[(cur->k)%2])*(c-cur->data[(cur->k)%2])<min_distance)
            recur_search(cur->left,x,y,min_distance,guess);
    }
}

void BinaryDimonTree::clear(TreeNode *tmp){//递归删除树节点
    if(tmp==NULL) return;
    clear(tmp->left);
    clear(tmp->right);
    delete tmp;
    tmp=NULL;
}

BinaryDimonTree::~BinaryDimonTree()
{   clear(root);
    // TODO: Your code here
}
