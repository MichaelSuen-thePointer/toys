#pragma once
#ifndef ARRAY_H
#define ARRAY_H

#include "ListBase.h"
namespace pl
{

template<typename T, size_t VSize>
class Array: Object
{
protected:
    T buffer[VSize];

public:
    Array()
    {
    }

    constexpr size_t Count() const
    {
        return VSize;
    }

};

}
#endif // !ARRAY_H
