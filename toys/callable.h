#pragma once
#ifndef CALLABLE_H
#define CALLABLE_H
#include "Base.h"
#include <functional>
#include <memory>

namespace pl
{

template<typename TRet, typename... TArgs>
class Invoker: public Object
{
public:
    virtual TRet operator()(TArgs&&...) = 0;
};

template<typename TRet, typename... TArgs>
class StaticInvoker: public Invoker<TRet, TArgs...>
{
protected:
    std::function<TRet(TArgs...)> _function;
public:
    StaticInvoker(TRet(*function)(TArgs...))
        : _function(function)
    {
    }
    StaticInvoker(std::function<TRet(TArgs...)>& function)
        : _function(function)
    {
    }
    TRet operator()(TArgs... args) override
    {
        return _function(std::forward<TArgs>(args)...);
    }
};

template<typename T, typename TRet, typename... TArgs>
class MemberInvoker: public Invoker<TRet, TArgs...>
{
protected:
    T* _sender;
    R
};

}

#endif // !CALLABLE_H
