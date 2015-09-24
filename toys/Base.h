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

}
#endif // !BASE_H
