#include "Student.h"
#include <string>
#include <sstream>
#include "Class.h"
#include <iostream>

std::string Student::toString() const
{
    // TODO: uncomment the following code after implementing class Student.

    std::ostringstream oss;
    oss << "Student Information:"
        << "\n\tid: " << id
        << "\n\tname: " << name
        << "\n\tenrollment year: " << year
        << "\n\tdegree: " << (degree == graduate ? "graduate" : "undergraduate")
        << std::endl;
    return oss.str();


}

// TODO: implement functions which are declared in Student.h.
void Student::addClass(Class* c){
    c->addStudent(*this);
    classes.push_back(c);
}

double Graduate::getGrade() {
    double sum=0.0;
    double sump=0.0;
    double grd;
    double gpa;
    for (std::vector<Class*>::iterator it = classes.begin();
         it != classes.end();
         ++it) {
        double scr=((*it)->getStudentWrapper(this->id)).getScore();
        //std::cout<<scr<<std::endl;
        if (scr>=90.0&&scr<=100.0) {
            grd=4.0;
        } else if (scr>=80.0&&scr<=89.0) {
            grd=3.5;
        } else if (scr>=70.0&&scr<=79.0) {
            grd=3.0;
        } else if (scr>=60&&scr<=69.0) {
            grd=2.5;
        } else {
            grd=2.0;}
        sump+=(*it)->point;
        sum+=(*it)->point*grd;
        //std::cout<<sump<<' '<<sum<<std::endl;
        }
    gpa=sum/sump;
    return gpa;
}



double Undergraduate::getGrade() {
    double sum=0.0;
    double sump=0.0;
    double gpa;
    for (std::vector<Class*>::iterator it = classes.begin();
         it != classes.end();
         ++it) {
        double scr=((*it)->getStudentWrapper(this->id)).getScore();
        scr=scr/20;
        //std::cout<<scr<<std::endl;
        sump+=(*it)->point;
        sum+=(*it)->point*scr;
        //std::cout<<sump<<' '<<sum<<std::endl;
    }
    gpa=sum/sump;
    return gpa;

}