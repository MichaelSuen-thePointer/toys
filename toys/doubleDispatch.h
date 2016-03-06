#pragma once
#ifndef DOUBLE_DISPATCH_H
#define DOUBLE_DISPATCH_H

#include <iostream>
#include <functional>

class Int;
class Double;
class String;

class SampleOperation
{
public:
    static void operate(const Int& a, const Int& b)
    {
        std::cout << "Int , Int" << std::endl;
    }

    static void operate(const Int& a, const Double& b)
    {
        std::cout << "Int , Double" << std::endl;
    }

    static void operate(const Int& a, const String& b)
    {
        std::cout << "Int , String" << std::endl;
    }
};

class Value
{
public:
    virtual void double_dispatch_operation(Value* value) = 0;
    virtual void double_dispatch_operation(const Int& value) = 0;
    virtual void double_dispatch_operation(const Double& value) = 0;
    virtual void double_dispatch_operation(const String& value) = 0;
};

class Int: public Value
{
public:
    virtual void double_dispatch_operation(Value* value) override
    {
        value->double_dispatch_operation(*this);
    }
    virtual void double_dispatch_operation(const Int& value) override
    {
        SampleOperation::operate(value, *this);
    }
    virtual void double_dispatch_operation(const Double& value) override
    {
        throw std::logic_error("not implemented");
    }
    virtual void double_dispatch_operation(const String& value) override
    {
        throw std::logic_error("not implemented");
    }
};

class Double: public Value
{
public:
    virtual void double_dispatch_operation(Value* value) override
    {
        value->double_dispatch_operation(*this);
    }
    virtual void double_dispatch_operation(const Int& value) override
    {
        SampleOperation::operate(value, *this);
    }
    virtual void double_dispatch_operation(const Double& value) override
    {
        throw std::logic_error("not implemented");
    }
    virtual void double_dispatch_operation(const String& value) override
    {
        throw std::logic_error("not implemented");
    }
};

class String: public Value
{
public:
    virtual void double_dispatch_operation(Value* value) override
    {
        value->double_dispatch_operation(*this);
    }
    virtual void double_dispatch_operation(const Int& value) override
    {
        SampleOperation::operate(value, *this);
    }
    virtual void double_dispatch_operation(const Double& value) override
    {
        throw std::logic_error("not implemented");
    }
    virtual void double_dispatch_operation(const String& value) override
    {
        throw std::logic_error("not implemented");
    }
};



#endif // !DOUBLE_DISPATCH_H
