#pragma once
#include <type_traits>
#include "tupleext.h"
#include <functional>

namespace mq
{

template<size_t idx>
struct placeholder
{
};

template<size_t idx>
constexpr auto ph = placeholder<idx>{};

namespace detail
{

template<class TRet, class TFunc, class... TCaptures>
class binder;

template<class TCapture, class TParams>
struct do_select_param
{
    decltype(auto) operator()(TCapture& capture, TParams&&)
    {
        return capture;
    }
};

template<size_t idx, class TParams>
struct do_select_param<placeholder<idx>, TParams>
{
    decltype(auto) operator()(placeholder<idx>, TParams&& params)
    {
        return std::get<idx>(std::move(params));
    }
};

template<class T, class TParams>
struct do_select_param<std::reference_wrapper<T>, TParams>
{
    decltype(auto) operator()(std::reference_wrapper<T>& capture, TParams&&)
    {
        return capture.get();
    }
};

template<class TFunc, class... TCaptures, class TParams>
struct do_select_param<binder<TFunc, TCaptures...>, TParams>
{
    decltype(auto) operator()(binder<TFunc, TCaptures...>& binder, TParams&& params)
    {
        return apply(binder, std::move(params));
    }
};

template<class TCapture, class TParams>
decltype(auto) select_param(TCapture& capture, TParams&& params)
{
    return do_select_param<TCapture, TParams>{}(capture, std::move(params));
}

template<size_t... idx, class TFunc, class TCaptures, class TParams>
decltype(auto) bind_invoke(std::index_sequence<idx...>, TFunc& func, TCaptures& captures, TParams&& params)
{
    return std::invoke(func, select_param(std::get<idx>(captures), std::move(params))...);
}

template<class TRet, class TFunc, class... TCaptures>
class binder
{
    using seq = std::index_sequence_for<TCaptures...>;
    using captures = std::tuple<std::decay_t<TCaptures>...>;
    using func = std::decay_t<TFunc>;

    func _func;
    captures _captures;
public:
    explicit binder(TFunc&& func, TCaptures&&... captures)
        : _func(std::forward<TFunc>(func))
        , _captures(std::forward<TCaptures>(captures)...)
    {
    }

    template<class... TParams>
    TRet operator()(TParams&&... params)
    {
        return bind_invoke(seq{}, _func, _captures, std::forward_as_tuple(std::forward<TParams>(params)...));
    }

    template<class... TParams>
    TRet operator()(TParams&&... params)const
    {
        return bind_invoke(seq{}, _func, _captures, std::forward_as_tuple(std::forward<TParams>(params)...));
    }
};

struct auto_return_type
{
};

template<class TFunc, class... TCaptures>
class binder<auto_return_type, TFunc, TCaptures...>
{
    using seq = std::index_sequence_for<TCaptures...>;
    using captures = std::tuple<std::decay_t<TCaptures>...>;
    using func = std::decay_t<TFunc>;

    func _func;
    captures _captures;
public:
    explicit binder(TFunc&& func, TCaptures&&... captures)
        : _func(std::forward<TFunc>(func))
        , _captures(std::forward<TCaptures>(captures)...)
    {
    }

    template<class... TParams>
    decltype(auto) operator()(TParams&&... params)
    {
        return bind_invoke(seq{}, _func, _captures, std::forward_as_tuple(std::forward<TParams>(params)...));
    }

    template<class... TParams>
    decltype(auto) operator()(TParams&&... params)const
    {
        return bind_invoke(seq{}, _func, _captures, std::forward_as_tuple(std::forward<TParams>(params)...));
    }
};

} //namespace detail

template<class TFunc, class... TCaptures>
decltype(auto) bind(TFunc&& func, TCaptures&&... captures)
{
    return detail::binder<detail::auto_return_type, TFunc, TCaptures...>{ std::forward<TFunc>(func), std::forward<TCaptures>(captures)... };
}

template<class TRet, class TFunc, class... TCaptures>
decltype(auto) bind(TFunc&& func, TCaptures&&... captures)
{
    return detail::binder<TRet, TFunc, TCaptures...>{ std::forward<TFunc>(func), std::forward<TCaptures>(captures)... };
}


}
