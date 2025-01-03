#ifndef STUDENT_H_
#define STUDENT_H_

#include <string>
#include <vector>
#include "Class.h"
class Class;

enum Degree {
    undergraduate,
    graduate
};

class Student {
    // TODO: implement class Student.
private:
    std::string name;
    std::string year;
    Degree degree;
protected:
    std::vector<Class*> classes;
public:
    const std::string id;
    std::string toString() const;
    virtual double getGrade()=0;
    void addClass(Class* c);
    Student(std::string a,std::string b,std::string c,Degree d):id(a),degree(d){
        name=b;
        year=c;

    }
};


// TODO: implement class Graduate.
class Graduate:public Student {
public:
    double getGrade();
    Graduate(std::string a,std::string b,std::string c):Student(a,b,c,graduate){

    }
};




// TODO: implement class Undergraduate.
class Undergraduate:public Student {
public:
    double getGrade();
    Undergraduate(std::string a,std::string b,std::string c):Student(a,b,c,undergraduate){

    }
};


class StudentWrapper {
private:
    const Student &student;
    double score = 0.0;
public:
    const std::string id;
    // TODO: fix error
    StudentWrapper(const std::string &id, const Student &student):student(student),id(id) {

    }

    void setScore(double score)
    {
        if (score < 0 || score > 100)
            throw "Invalid Score!";
        this->score = score;
    }

    double getScore() const
    {
        return this->score;
    }

    std::string toString() const
    {
        return student.toString();
    }
};

#endif // STUDENT_H_
