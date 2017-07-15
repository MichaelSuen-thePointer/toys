#pragma once
#include <ratio>

namespace mq
{

template<class T>
struct _rw //ratio wrapper
{
    static constexpr auto num = T::num;
    static constexpr auto den = T::den;
    using type = typename T::type;
};

template<intmax_t num, intmax_t den = 1>
auto ratio = _rw<std::ratio<num, den>>{};

template<class A, class B>
decltype(auto) operator+(_rw<A>, _rw<B>)
{
    return _rw<std::ratio_add<A, B>>{};
}

template<class A, class B>
decltype(auto) operator-(_rw<A>, _rw<B>)
{
    return _rw<std::ratio_subtract<A, B>>{};
}

template<class A, class B>
decltype(auto) operator*(_rw<A>, _rw<B>)
{
    return _rw<std::ratio_multiply<A, B>>{};
}

template<class A, class B>
decltype(auto) operator/(_rw<A>, _rw<B>)
{
    return _rw<std::ratio_divide<A, B>>{};
}

template<class A, class B>
decltype(auto) operator==(_rw<A>, _rw<B>)
{
    return std::ratio_equal<A, B>{};
}

template<class A, class B>
decltype(auto) operator!=(_rw<A>, _rw<B>)
{
    return std::ratio_not_equal<A, B>{};
}

template<class A, class B>
decltype(auto) operator<(_rw<A>, _rw<B>)
{
    return std::ratio_less<A, B>{};
}

template<class A, class B>
decltype(auto) operator<=(_rw<A>, _rw<B>)
{
    return std::ratio_less_equal<A, B>{};
}

template<class A, class B>
decltype(auto) operator>(_rw<A>, _rw<B>)
{
    return std::ratio_greater<A, B>{};
}

template<class A, class B>
decltype(auto) operator>=(_rw<A>, _rw<B>)
{
    return std::ratio_greater_equal<A, B>{};
}

}