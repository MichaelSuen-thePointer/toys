#pragma once
#ifndef DOUBLE_DISPATCH_H
#define DOUBLE_DISPATCH_H

#include <iostream>
#include <functional>

class Person
{

};

class Student : public Person
{

};

class Teacher : public Person
{
    
};

class Officer : public Person
{
    
};

class Fuck
{
public:
    void fuck(Person& p1, Person& p2)
    {
        std::cout << "person fuck person\n";
    }

    void fuck(Student& p1, Teacher& p2)
    {
        std::cout << "student fuck teacher\n";
    }

    void fuck(Officer& p1, Student& p2)
    {
        std::cout << "officer fuck student\n";
    }

    void fuck(Teacher& p1, Person& p2)
    {
        std::cout << "teacher fuck person\n";
    }
};

#endif // !DOUBLE_DISPATCH_H
