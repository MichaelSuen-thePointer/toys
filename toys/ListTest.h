#pragma once
#include <vld.h>
#include "List.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
using namespace pl;
using namespace pl::container;

struct obj
{
    int placeholder;
};
struct no_default_ctor
{
    int placeholder;
    no_default_ctor() = delete;
    no_default_ctor(int a)
    {
        placeholder = a;
    }
};
struct ctorException
{
public:
    static int count;
    int* ptr;
    ctorException()
    {
        ptr = new int;
        count++;
        if (count == 10)
        {
            delete ptr;
            count = 0;
            throw "here boom";
        }
    }
    ctorException(const ctorException& v)
    {
        ptr = new int;
        count++;
        if (count == 10)
        {
            delete ptr;
            count = 0;
            throw "here boom";
        }
    }
    ~ctorException()
    {
        delete ptr;
    }
};
struct random_exception
{
    static int count;
    int placeholder;
    int* mem;
    void rand_boom()
    {
        double value = rand() / (double)RAND_MAX;
        if (0)
        {
            throw "boom";
        }
    }
    ~random_exception()
    {
        placeholder = -placeholder;
        if (mem)
        {
            delete mem;
        }
        mem = nullptr;
    }
    random_exception(int v)
    {
        placeholder = v;
        mem = new int;
    }
    random_exception()
    {
        placeholder = count++;
        mem = new int;
        //rand_boom();
    }
    random_exception(const random_exception& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        mem = new int;
        placeholder = v.placeholder;
    }
    random_exception(random_exception&& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        mem = v.mem;
        v.mem = nullptr;
        placeholder = v.placeholder;
    }
    random_exception& operator=(const random_exception& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
        mem = new int;

        return *this;
    }
    random_exception& operator=(random_exception&& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
        mem = v.mem;
        v.mem = nullptr;

        return *this;
    }
};
int random_exception::count = 0;
int ctorException::count = 0;

template<>
struct IsPOD<obj>
{
    using Result = FalseType;
};

template<>
struct IsPOD<ctorException>
{
    using Result = FalseType;
};

template<>
struct IsPOD<random_exception>
{
    using Result = FalseType;
};

int main()
{
    time_t t;
    time(&t);
    srand(unsigned(((unsigned long long)t >> 32) ^ (t & 0xFFFFFFFF)));

    random_exception relist[50];
    std::vector<int> a;
    List<random_exception> list(0);
    try
    {
        for (int i = 0; i < 50; i++)
        {
            list.AddBack(random_exception());
            a.push_back(1);
            std::cout << list.Capacity() << ' ' << a.capacity() << std::endl;
        }

        list.Insert(5, relist, relist + 30);
        std::cout << list.Count() << ' ' << list.Capacity() << std::endl;
    }
    catch (...)
    {
        std::cout << "boom" << std::endl;
    }
    std::cin.get();
}