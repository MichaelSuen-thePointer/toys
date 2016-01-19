#pragma once
#ifndef BASE_H
#define BASE_H

#include <memory.h>
#include <cassert>
#include <intrin.h>
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
    NotCopyable(const NotCopyable&) = delete;
    NotCopyable(NotCopyable&&) = delete;
    NotCopyable& operator=(const NotCopyable&) = delete;
    NotCopyable& operator=(NotCopyable&&) = delete;
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

template<typename T>
inline T Max(const T& left, const T& right)
{
    return left < right ? right : left;
}

template<typename T>
inline T Min(const T& left, const T& right)
{
    return left < right ? left : right;
}

#define LOCKED_INC(ptr) _InterlockedIncrement((volatile long*)ptr)
#define LOCKED_DEC(ptr) _InterlockedDecrement((volatile long*)ptr)

#if defined DEBUG || !defined NDEBUG
#define PLDEBUG 1



#define CHECK_ERROR(cond, msg) do{if(!(cond)) throw Error(msg, __LINE__);}while(0)
#define FAST_FAIL do {throw Error("Fast fail called.", __LINE__);}while(0)
#define FAIL(message) do {throw Error(message, __LINE__);}while(0)
#else
#define CHECK_ERROR(cond, msg)
#endif



}
#endif // !BASE_H
