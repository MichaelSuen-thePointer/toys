#include "Function.h"

using namespace pl;

void function(void)
{
    
}

struct callable
{
    void operator()(void)
    {

    }
};

struct test
{
    void memfunc(void)
    {

    }
};
int main()
{
    test a;
    callable c;
    auto f1 = Callable<void(void)>([]() {});
    auto f2 = Callable<void(void)>(&a, &test::memfunc);
    auto f3 = Callable<void(void)>(c);
    f1();
    f2();
    f3();
}