#pragma once
#ifndef BASE_H
#define BASE_H

#include <memory.h>
#include <cassert>

namespace pl
{

class Object
{
public:
    virtual ~Object() {}
};

class NotCopyable
{
protected:
    NotCopyable() {}
    ~NotCopyable() {}
private:
    NotCopyable(const NotCopyable&) {}
    NotCopyable(NotCopyable&&) {}
    NotCopyable& operator==(const NotCopyable&) {}
    NotCopyable& operator==(NotCopyable&&) {}
};

class Interface: NotCopyable
{
public:
    virtual ~Interface() {}
};

using byte = unsigned char;

template<typename T>
bool GetBit(T value, size_t bit)
{
    assert(bit < sizeof(T) * 8);
    return !!((value >> bit) & 1);
}
}
#endif // !BASE_H
