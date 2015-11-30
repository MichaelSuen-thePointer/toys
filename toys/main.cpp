#include <vld.h>
#include "String.h"
#include <iostream>
using namespace pl;

int main()
{
    {
        AnsiString a = "   0123456789   ";
        AnsiString b(a, 2, 4);
        AnsiString c(a);
        AnsiString d = a + b;
        std::cout << a.RawString() << std::endl;
        std::cout << b.RawString() << std::endl;
        std::cout << c.RawString() << std::endl;
        std::cout << d.RawString() << std::endl;
        for (size_t i = 0; i < c.Length(); i++)
        {
            d = a + c.SubStr(i);
            std::cout << d.RawString() << std::endl;
        }
        AnsiString e = a.TrimStart();
        AnsiString f = e.TrimEnd();

        std::cout << a.RawString() << std::endl;
        std::cout << b.RawString() << std::endl;
        std::cout << c.RawString() << std::endl;
        std::cout << e.RawString() << std::endl;
        std::cout << f.RawString() << std::endl;

    }

}
