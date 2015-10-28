#include "List2.h"
#include <cassert>
#include <vector>
using namespace pl;

int main()
{
    List<int> list;
    for (int i = 0; i < 20; i++)
    {
        list.Push(i);
    }
    for (int i = 0; i < 20; i++)
    {
        assert(list[i] == i);
    }
    

}