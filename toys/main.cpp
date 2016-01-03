#include <iostream>
#include "Event.h"

using namespace pl;
using namespace std;
void function(void)
{
    cout << "function" << endl;
}

struct callable
{
    void operator()(void)
    {
        cout << "callable" << endl;
    }
};

struct test
{
    void memfunc(void)
    {
        cout << "memfunc" << endl;
    }
};
using FooEvent = Event<>;
int main()
{
    FooEvent OnFooEvent;
    callable functor;
    test testa;

    ios::sync_with_stdio(false);

    OnFooEvent += FooEvent::Handler(function);
    OnFooEvent += FooEvent::Handler(functor);
    OnFooEvent += EventHandler<>(&testa, &test::memfunc);

    OnFooEvent();
}