#pragma once
#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include "Base.h"
#include "Template.h"

namespace pl
{
struct Int32: Object
{
private:
    __int32 _value;
public:
    Int32(int value) noexcept
        : _value(value)
    {
    }
    Int32(const Int32& value) noexcept
        : _value(value._value)
    {
    }
    operator int()
    {
        return _value;
    }
    static constexpr int MaxValue() noexcept
    {
        return 0xEFFFFFFF;
    }
};


}
#endif // !BASE_TYPE_H
