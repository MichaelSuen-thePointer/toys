#include "Property.h"
using namespace pl;
using namespace tml;

class PropertyTest
{
public:
    Property<int> length = {
        10,                                  //value
        [](const int& _value) {return 10; }, //getter
        DefaultSetter<int>                   //setter
    };
};

int main()
{
    PropertyTest test;
    test.length = 10;
    int a = test.length;
}
