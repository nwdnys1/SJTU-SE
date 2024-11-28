#include <iostream>
#include <sstream>
#include "Editor.h"

using namespace std;

Editor::Editor()
{
    buffer = new Buffer();
}
Editor::~Editor()
{
    // TODO: Implement destructor
    delete buffer;
}

void Editor::run()
{
    string cmd;
    while (true)
    {
        cout << "cmd> ";
        cout.flush();
        getline(cin, cmd);
        if (cmd == "Q")
            break;
        try {
            dispatchCmd(cmd);
        } catch (const char *e) {
            cout << "? " << e << endl;
        } catch (const out_of_range &oor) {
            cout << "? " << oor.what() << endl;
        } catch (const range_error &re) {
            cout << "? " << re.what() << endl;
        }
    }
}
void Editor::cmdAppend()
{
    cout << "It's input mode now. Quit with a line with a single dot(.)" << endl;
    // TODO: finish cmdAppend.
    while (true)
    {   string text;
        getline(cin, text);
        if (text == ".")
            break;
        else{
            buffer->appendLine(text);
        }
    }
}

void Editor::cmdInsert()
{
    cout << "It's input mode now. Quit with a line with a single dot(.)" << endl;
    bool firstLine = true;
    while (true)
    {
        string text;
        getline(cin, text);
        if (text == ".")
            break;
        if (firstLine) {
            buffer->insertLine(text);
            firstLine = false;
        }  else {
            buffer->appendLine(text);
        }
    }
}

void Editor::cmdDelete(int start, int end)
{   if(start>end){
        throw range_error("Delete range error");
        //cout<<"? Delete range error"<<endl;
    }
    else{
        if(start<0||end>buffer->currentlength){
            throw out_of_range("Line number out of range");
            //cout<<"? Line number out of range"<<endl;
        }
        else
        buffer->deleteLines(start, end);
    }
}

void Editor::cmdNull(int line)
{   if(line<0||line>buffer->currentlength){
        throw out_of_range("Line number out of range");
        //cout<<"? Line number out of range"<<endl;
    }
    else{
        cout <<buffer->moveToLine(line)<< endl;
    }

}

void Editor::cmdNumber(int start, int end)
{   if(start>end){
        throw range_error("Number range error");
        //cout<<"? Number range error"<<endl;
    }
    else{
        if(start<0||end>buffer->currentlength){
            throw out_of_range("Line number out of range");
            // cout<<"? Line number out of range"<<endl;
        }
        else
            buffer->showLines(start, end);
    }

}

void Editor::cmdWrite(const string &filename)
{
    buffer->writeToFile(filename);
}

void Editor::dispatchCmd(const string &cmd)
{
    if (cmd == "a") {
        cmdAppend();
        return;
    }
    if (cmd == "i") {
        cmdInsert();
        return;
    }
    if (cmd[0] == 'w') {
        // TODO: call cmdWrite with proper arguments
        string file;
        char a;
        stringstream ss(cmd);
        ss>>a>>file;
        if (file.empty()) {cout<<"? Filename not specified"<<endl;return;}
        else cmdWrite(file);
        return;
    }
    // TODO: handle special case "1,$n".
    int start, end;
    char comma, type = ' ';
    stringstream ss(cmd);
    ss >> start;
    if (ss.eof()) {
        cmdNull(start);
        return;
    }
    ss >> comma >> end >> type;
    if (ss.good()) {
        if (type == 'n') {

            cmdNumber(start, end);
            return;
        } else if (type == 'd') {
            cmdDelete(start, end);
            return;
        }
    }
    stringstream ss2(cmd);
    char tmp=' ';
    ss2>>start>>comma>>tmp>>type;
    if(start==1&&comma==','&&tmp=='$'&&type=='n'){
        cmdNumber(start,buffer->currentlength);
        return;
    }
    throw "Bad/Unknown command";
}
