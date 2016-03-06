#pragma once
#ifndef SORTEDLIST_H
#define SORTEDLIST_H

#include "List.h"
namespace pl
{
namespace container
{

template<typename T>
class SortedList: public List<T>
{
protected:
    using base = List<T>;

    SortedList()
        : base()
    {
    }

    SortedList(size_t count)
        : base(count)
    {
    }

    SortedList(size_t count, const T& value)
        : base(count, value)
    {
    }
    
    SortedList(const T* begin, const T* end)
        : base(begin, end)
    {
        //sort it
    }


};

}
}
#endif // !SORTEDLIST_H
