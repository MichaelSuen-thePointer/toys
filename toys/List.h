#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
namespace pl
{
namespace container
{

template<typename T>
class List: public Object
{
protected:
    T* buffer;
    T* tail;
    T* end;
    void InternalFill(const T& value, size_t count, TrueType)
    {

    }
public:
    List(size_t size)
        : buffer(new T[size])
        , tail(buffer + size)
        , end(buffer)
    {
    }

    List(size_t size, const T& value)
        : buffer(new T[size])
        , tail(buffer + size)
        , end(tail)
    {
        InternalFill(value, size, IsPOD<T>::Result())
    }
};

}
}
#endif // !LIST_H
