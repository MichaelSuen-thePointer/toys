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

struct Error
{
    const char* Message;
    int Line;
    Error(const char* message, int line)
        : Message(message)
        , Line(line)
    {
    }
};

#define FAST_FAIL do {throw Error("Fast fail called.", __LINE__);}while(0)
#define FAIL(message) do {throw Error(message, __LINE__);}while(0)

#if defined DEBUG || !defined NDEBUG
#define PLDEBUG 1
#endif

}
#endif // !BASE_H
