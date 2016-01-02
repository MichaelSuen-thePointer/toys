#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "Base.h"
#include <functional>
#include <memory>
#include <vector>
namespace pl
{
template<typename T>
class Event;

template<typename T>
class EventHandler;



template<typename... TArgs>
class Event<void(TArgs...)>: public Object, private NotCopyable
{
public:
    class EventHandler: Object
    {
    public:
        template<typename T>
        std::shared_ptr<EventHandler> Get(void(*handler)(TArgs...))
        {
            return std::make_shared<StaticFuncHandler<TArgs...>>(handler);
        }
        virtual void operator()(TArgs...) = 0;
    };
    class StaticFuncHandler : EventHandler
    {
    protected:
        std::function<void(TArgs...)> function;
    public:
        StaticFuncHandler(const std::function<void(TArgs...)>& handler)
            : function(handler)
        {
        }
        StaticFuncHandler(void(*handler)(TArgs...))
            : function(handler)
        {
        }
        void operator()(TArgs... args) const override
        {
            function(std::forward<TArgs>(args)...);
        }
    };
    class MemberFuncHandler: EventHandler<TArgs...>
    {
    protected:
        
    public:
        template<typename T>
        Handler(T* sender, void(T::*function)(TArgs...))
        {

        }
    };
protected:
    std::vector<Handler> handlers;
public:
    Event()
        : handlers()
    {
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
        for (auto& handler : handlers)
        {
            handler(std::forward<TArgs>(args)...);
        }
    }
};

}
#endif // !EVENT_H
