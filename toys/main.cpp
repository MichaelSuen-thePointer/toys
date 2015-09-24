#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include "Tree.h"
#include "List.h"
using namespace pl;
using namespace pl::container;
int main()
{
    FixSizeList<int> alist(10);
    FixSizeList<int> blist(alist);
}