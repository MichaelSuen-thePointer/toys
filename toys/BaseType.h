#pragma once
#ifndef BASE_TYPE_H
#define BASE_TYPE_H

#include "Base.h"
#include "Template.h"

namespace pl
{
class Int32: Object
{
public:
    friend Int32 operator+(const Int32&, const Int32&);
    friend Int32 operator-(const Int32&, const Int32&);
    friend Int32 operator*(const Int32&, const Int32&);
    friend Int32 operator/(const Int32&, const Int32&);
protected:
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
    explicit operator int() noexcept
    {
        return _value;
    }
    static constexpr int MaxValue() noexcept
    {
        return 0xEFFFFFFF;
    }
    static constexpr int MinValue() noexcept
    {
        return 0x80000000;
    }
};

Int32 operator+(const Int32& a, const Int32& b)
{
    return Int32(a._value + b._value);
}
Int32 operator-(const Int32& a, const Int32& b)
{
    return Int32(a._value - b._value);
}
Int32 operator*(const Int32& a, const Int32& b)
{
    return Int32(a._value * b._value);
}
Int32 operator/(const Int32& a, const Int32& b)
{
    return Int32(a._value / b._value);
}

class Float32: public Object
{
protected:
    double _value;
public:
    Float32(double value) noexcept
        : _value(value)
    {
    }
    Float32(const Float32& value) noexcept
        : _value(value._value)
    {
    }
    explicit operator double() noexcept
    {
        return _value;
    }


};

}
#endif // !BASE_TYPE_H
