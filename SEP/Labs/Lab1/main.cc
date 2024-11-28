#include <vector>
#include <iostream>
#include <fstream>
#include "Class.h"
#include "Student.h"
#include <iomanip>
using namespace std;

class AppX {
private:
    vector<Student *> studentVec;
    vector<Class *> classVec;

    void loadFiles();
    void inputScore();
    void printAvgScore();
    void printGpa();
    void printclass();

public:
    AppX();
    ~AppX();
    int run();
};

void AppX::printclass() {
    for (std::vector<Class*>::iterator it = classVec.begin();
         it != classVec.end();
         ++it) {
        cout<<(*it)->name<<endl<<(*it)->point<<endl;
        (*it)->printstudent();
    }

}



AppX::~AppX()
{
    // You can use the traditional loop, which is more clear.
    for (vector<Class *>::iterator it = classVec.begin();
         it != classVec.end();
         ++it) {
        if (*it) delete (*it);
    }
    // You can use modern and simpler loops only if you know what it is doing.
    for (const auto &s: studentVec) {
        if (s) delete (s);
    }
}

AppX::AppX()
{
    loadFiles();
}

void AppX::loadFiles()
{
    string line;
    size_t pos1, pos2;
    vector<string> bufv;
    Student *st = nullptr;
    string clsname;
    int point;
    Class *cl = nullptr;

    // Open a file as a input stream.
    ifstream stfile("./Students.txt");

    while (getline(stfile, line)) {
        if (line.empty()) // It's an empty line.
            continue;
        if (line[0] == '#') // It's a comment line.
            continue;

        // The bufv vector stores each columnes in the line.
        bufv.clear();
        // Split the line into columes.
        //   pos1: begining the the column.
        //   pos2: end of the column.
        pos1 = 0;
        while (true) {
            pos2 = line.find(';', pos1 + 1);
            if (pos2 == string::npos) { // No more columns.
                // Save the last column (pos1 ~ eol).
                bufv.push_back(line.substr(pos1, string::npos));
                break;
            } else {
                // Save the column (pos1 ~ pos2).
                bufv.push_back(line.substr(pos1, pos2 - pos1));
            }
            pos1 = pos2 + 1;
        }

        // TODO: uncomment next lines after implementing class Undergraduate
        // and Graduate.

        if (bufv[3] == "U")
            st = new Undergraduate(bufv[0], bufv[1], bufv[2]);
        else
            st = new Graduate(bufv[0], bufv[1], bufv[2]);


        studentVec.push_back(st);
    }
    stfile.close();

    // TODO: load data from ./Classes.txt and push objects to the vector.
    // Hint: how is student information read?
    ifstream stfile2("./Classes.txt");

    while (getline(stfile2, line)) {
        if(line[0]=='C'){
            string line2;
            getline(stfile2,line2);
            cl = new Class(line.substr(6), line2[7]-'0');
            classVec.push_back(cl);
            while(getline(stfile2,line2)){
                if (line2.empty()) // It's an empty line.
                    break;
                for (std::vector<Student*>::iterator it = studentVec.begin();
                         it != studentVec.end();
                         ++it)
                        {if(line2==(*it)->id){
                            (*it)->addClass(cl);}}}}
        }

    stfile2.close();
}

void AppX::inputScore()
{
    // TODO: implement inputScore.
    // Hint: Take a look at printAvgScore().
    string sbuf;
    Class *cl;
    Student *st;
    double scr;

    while (true) {
        cout << "Please input the class name (or input q to quit): ";
        cin >> sbuf;
        if (sbuf == "q")
            break;

        cl = nullptr;
        for (vector<Class *>::iterator it = classVec.begin();
             it != classVec.end();
             ++it) {
            if ((*it)->name == sbuf) {
                cl = *it;
                break;
            }
        }
        if (cl == nullptr) {
            cout << "No match class!" << endl;
            continue;
        }
        while(1){
        cout << "Please input the student id (or input q to quit): ";
        cin >> sbuf;
        if (sbuf == "q")
            break;

        st = nullptr;
        for (vector<Student*>::iterator it = studentVec.begin();
             it != studentVec.end();
             ++it) {
            if ((*it)->id == sbuf) {
                st = *it;
                break;
            }
        }
        if (st == nullptr) {
            cout << "No match student!" << endl;
            continue;
        }
        //st->toString();
        cout << "Please input the student score : ";
        cin >> scr;
        if(scr>100||scr<0) {
            cout << "Wrong score!" << endl;
            continue;
        }
        (cl->getStudentWrapper(st->id)).setScore(scr);
        //cout << "The score is already set. ";}
    }
}}

void AppX::printAvgScore()
{
    string sbuf;
    Class *cl;
    double avg;

    while (true) {
        cout << "Please input the class name (or input q to quit): ";
        cin >> sbuf;
        if (sbuf == "q")
            break;

        cl = nullptr;
        for (vector<Class *>::iterator it = classVec.begin();
             it != classVec.end();
             ++it) {
            if ((*it)->name == sbuf) {
                cl = *it;
                break;
            }
        }
        if (cl == nullptr) {
            cout << "No match class!" << endl;
            continue;
        }

        avg = cl->getAvgScore();
        cout<<fixed<<setprecision(2)<< "The average score is: " << avg << endl;
    }
}

void AppX::printGpa()
{
    // TODO: implement printGpa.
    // Hint: Take a look at printAvgScore().
    string sbuf;
    Student *st;
    double grd;

    while (true) {
        cout << "Please input the student id (or input q to quit): ";
        cin >> sbuf;
        if (sbuf == "q")
            break;

        st = nullptr;
        for (vector<Student *>::iterator it = studentVec.begin();
             it != studentVec.end();
             ++it) {
            cout<<(*it)->id<<endl;
            if ((*it)->id == sbuf) {
                st = *it;
                break;
            }
        }
        if (st == nullptr) {
            cout << "No match student!" << endl;
            continue;
        }
        grd = st->getGrade();
        cout <<fixed<<setprecision(2)<< "GPA = " << grd << endl;
    }


}

int AppX::run()
{   char cmd;
    while (true) {
        cout << "Command menu:\n"
            << "\ti: Input score\n"
            << "\ta: Compute average score of a class\n"
            << "\tg: Compute grade of a student\n"
            << "\tq: Quit\n"
            << "Please input the command: ";
        cin >> cmd;
        if (cmd == 'i') {
            inputScore();
        } else if (cmd == 'a') {
            printAvgScore();
        } else if (cmd == 'g') {
            printGpa();
        } else if (cmd == 'q') {
            break;
        } else {
            cout << "Invalid command!\n" << endl;
        }
    }
    return 0;
}

int main()
{
    AppX app;
    return app.run();

}
