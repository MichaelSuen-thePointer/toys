#include <vld.h>
#include "List.h"

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


int main()
{
    obj objarray[50];
    List<obj> objList(5);

    objList.Insert(3, objarray, objarray + 30);
    
    objList.Insert(4, obj());
    objList.Insert(4, objarray[0]);
    objList.AddBack(objarray[1]);
    objList.AddBack(obj());
    objList.RemoveBack();

}