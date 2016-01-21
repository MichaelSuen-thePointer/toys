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
    int placeholder;
    void rand_boom()
    {
        double value = rand() / (double)RAND_MAX;
        if (value < 0.01)
        {
            throw value;
        }
        else
        {
            std::cout << value << "--" << std::endl;
        }
    }
    random_exception(int v)
    {
        placeholder = v;
    }
    random_exception()
    {
        //rand_boom();
    }
    random_exception(const random_exception& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
    }
    random_exception(random_exception&& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
    }
    random_exception& operator=(const random_exception& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
        return *this;
    }
    random_exception& operator=(random_exception&& v)
    {
        if (v.placeholder != 1)
        {
            rand_boom();
        }
        placeholder = v.placeholder;
        return *this;
    }
};
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
    List<random_exception> list(1, random_exception(1));
    try
    {
        for (int i = 0; i < 50; i++)
        {
            list.AddBack(random_exception(1));
            a.push_back(1);
            std::cout << list.Capacity() << ' ' << a.capacity() << std::endl;
        }

        list.Insert(5, relist, relist + 30);
        std::cout << list.Count() << ' ' << list.Capacity() << std::endl;
    }
    catch (double v)
    {
        std::cout << list.Count() << ' ' << list.Capacity() << std::endl;

        std::cout << "boom!!!!" << v << std::endl;
    }
    std::cin.get();
}