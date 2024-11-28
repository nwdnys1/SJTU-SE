#pragma once

#include <string>

using std::string;

class Buffer {
private:
    int currentLineNum;//当前行位置
    // TODO: add a List here for storing the input lines
    struct node{
        string data;
        node* prev,*next;
        node(const string& x,node *p=nullptr,node *n=nullptr){data=x;next=n;prev=p;}
        node():next(nullptr),prev(nullptr){}
        ~node(){}
    };
    node *head,*tail;
    node *move(int i)const{
        node *p=head;
        while(i-- >0)p=p->next;
        return p;
    }

public:
    int currentlength;
    Buffer();
    ~Buffer();

    void writeToFile(const string &filename) const;

    const string &moveToLine(int idx);

    void showLines(int from, int to);

    void deleteLines(int from, int to);
    void insertLine(const string &text);
    void appendLine(const string &text);
};
