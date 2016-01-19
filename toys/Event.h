#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "Base.h"
#include "Pointer.h"
#include "Function.h"
#include <map>

namespace pl
{
template<typename TResult, typename TCombinedResult>
class IResultCombiner: public virtual Interface
{
public:
    virtual void AddResult(const TResult& result) = 0;
    virtual TCombinedResult GetCombinedResult() = 0;
};

template<>
class IResultCombiner<void, void>: public virtual Interface
{
public:
    virtual void AddResult() = 0;
    virtual void GetCombinedResult() = 0;
};

class VoidResultCombiner: public virtual IResultCombiner<void, void>
{
public:
    void AddResult() override {}
    void GetCombinedResult() override {}
};

template<typename TRet, typename... TArgs>
struct EventHandler
{
    Callable<TRet(TArgs...)> handler;
    bool isValid;
};


class EventHandlerToken
{
public:

};


template<typename TSignature, typename TLabel, typename TCombiner>
class Event;

template<typename TRet, typename... TArgs, typename TLabel = int, typename TResultCombiner = VoidResultCombiner>
class Event<
    TRet(TArgs...), TLabel, TResultCombiner>: public Object, private NotCopyable
{
protected:
    std::map<TLabel, Callable<TRet(TArgs...)>> handlers;
public:
    using Handler = Callable<TRet(TArgs...)>;

    friend class Handler;

    Event()
        : handlers()
    {
    }
    template<class TClass>
    void Bind(TClass* object, void(TClass::*function)(TArgs...))
    {
        this->operator+=(Handler(object, function));
    }
    template<class TCallable>
    void Bind(TCallable& callable)
    {
        this->operator+=(Handler(callable));
    }
    void Bind(void(*function)(TArgs...))
    {
        this->operator+=(Handler(function));
    }
    template<class TClass>
    void Unbind(TClass* object, void(TClass::*function)(TArgs...))
    {
        this->operator-=(Handler(object, function));
    }
    template<class TCallable>
    void Unbind(TCallable& callable)
    {
        this->operator-=(Handler(callable));
    }
    void Unbind(void(*function)(TArgs...))
    {
        this->operator-=(Handler(function));
    }
    Event& operator+=(Handler& handler)
    {
        if (std::find(handlers.begin(), handlers.end(), handler) == handlers.end())
        {
            handlers.push_back(handler);
        }
        return *this;
    }
    Event& operator-=(Handler& handler)
    {
        auto place = std::find(handlers.begin(), handlers.end(), handler);
        if (place != handlers.end())
        {
            handlers.erase(place);
        }
        return *this;
    }
    void operator()(TArgs... args) const
    {
        for (auto&& handler : handlers)
        {
            handler(PerfectForward<TArgs>(args)...);
        }
    }
};

}
#endif // !EVENT_H
