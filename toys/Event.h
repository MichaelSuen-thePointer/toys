#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "Base.h"
#include "Pointer.h"
#include "Function.h"
#include <vector>

namespace pl
{

template<typename... TArgs>
using EventHandler = Callable<void(TArgs...)>;

template<typename... TArgs>
class Event: public Object, private NotCopyable
{
protected:
    std::vector<Callable<void(TArgs...)>> handlers;
public:
    using Handler = Callable<void(TArgs...)>;

    friend class Handler;

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
    void operator()(TArgs&&... args) const
    {
        for (auto&& handler : handlers)
        {
            handler(PerfectForward<TArgs>(args)...);
        }
    }
};

}
#endif // !EVENT_H
