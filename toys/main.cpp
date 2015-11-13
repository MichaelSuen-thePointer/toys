#include "Numbers.h"
#include <iostream>
using namespace pl;

int main()
{
    Number a = "B10000000";
    Number b = "B10000000";
    Number c = "0323546123567";
    
    std::cout << a << '\n';
    std::cout << b << '\n';
    //std::cout << c << '\n';
    //std::cout << -a << '\n';
    //std::cout << -b << '\n';
    //std::cout << -c << '\n';
    std::cout << a + b << '\n';
    //std::cout << b + c << '\n';
    //std::cout << a + -b << '\n';
}