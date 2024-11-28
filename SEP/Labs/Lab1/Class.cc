#include "Class.h"
#include <string>
#include "Student.h"
#include <iostream>

void Class::printstudent() {
    for (std::vector<StudentWrapper>::iterator it = students.begin();
         it != students.end();
         ++it) {
        std::cout<<(*it).id<<std::endl;
    }
}


void Class::addStudent(const Student &st)
{
    StudentWrapper sw(st.id, st);
    students.push_back(sw);
}

StudentWrapper &Class::getStudentWrapper(const std::string &studentId)
{
    for (std::vector<StudentWrapper>::iterator it = students.begin();
         it != students.end();
         ++it) {
        if (it->id == studentId)
            return *it;
    }
    throw "No matching student!";
}

double Class::getAvgScore()
{   double avg;
    double sum=0.0;
    double n=0.0;
    for (std::vector<StudentWrapper>::iterator it = students.begin();
         it != students.end();
         ++it) {
        sum+=(*it).getScore();
        n+=1;
    }
    avg=sum/n;
    // TODO: implement getAvgScore.
    return avg;
}
