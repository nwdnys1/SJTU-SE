#include <fstream>
#include <iostream>
#include "Buffer.h"

//TODO: your code here
//implement the functions in ListBuffer

Buffer::Buffer() {
    head=new node;
    tail=new node;
    head->next=tail;
    tail->prev=head;
    currentlength=0;
    currentLineNum=0;
}

Buffer::~Buffer() {
    node *p=head->next,*q;
    head->next=tail;
    tail->prev=head;
    while(p!=tail){
        q=p->next;
        delete p;
        p=q;
    }
    currentlength=0;
    delete head;delete tail;
}

void Buffer::writeToFile(const string &filename) const {
    string line;
    std::ofstream fout;
    fout.open(filename);
    int zishu=0;
    node*tmp=head->next;
    for(int i=0;i<currentlength;++i){
        fout<<tmp->data<<std::endl;
        zishu=zishu+1+(tmp->data).length();
        tmp=tmp->next;
    }
    fout.close();
    std::cout<<zishu<<" byte(s) written"<<std::endl;
}

void Buffer::showLines(int from, int to){
    currentLineNum=to;
    node *pos,*tmp;
    int num=from;
    pos=move(from);
    tmp=pos;
    for(int i=0;i<to-from+1;++i){
        std::cout<<num<<'\t'<<tmp->data<<std::endl;
        tmp=tmp->next;num+=1;
    }
}

void Buffer::deleteLines(int from, int to){
    node *pos,*tmp,*nxt;
    pos=move(from);
    tmp=pos;
    for(int i=0;i<to-from+1;++i){
        nxt=tmp->next;
        tmp->prev->next=tmp->next;
        tmp->next->prev=tmp->prev;
        delete tmp;
        tmp=nxt;
        --currentlength;
    }
    if(to==currentlength){
        currentLineNum=from-1;
    }
    else {
        currentLineNum=from;
    }
}

void Buffer::insertLine(const string &text){
    if (currentLineNum==0){
        appendLine(text);
    }
    else{
        node *pos,*tmp;
        pos=move(currentLineNum-1);
        tmp=new node(text,pos,pos->next);
        pos->next->prev=tmp;
        pos->next=tmp;
        ++currentlength;

    }
}

void Buffer::appendLine(const string &text){
    node *pos,*tmp;
    pos=move(currentLineNum);
    tmp=new node(text,pos,pos->next);
    pos->next->prev=tmp;
    pos->next=tmp;
    ++currentlength;
    ++currentLineNum;
}

const string &Buffer::moveToLine(int idx) {
    currentLineNum=idx;
    node *pos=move(currentLineNum);
    return pos->data;
}
