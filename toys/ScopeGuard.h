#pragma once
#include <type_traits>
#include <exception>

namespace mq
{
namespace detail
{
class UncaughtExceptionCounter
{
    int _exceptionCount;
public:
    UncaughtExceptionCounter()
        : _exceptionCount(std::uncaught_exceptions())
    {
    }
    bool newUncaughtException() const noexcept
    {
        return std::uncaught_exceptions() > _exceptionCount;
    }
};

template<class TCall>
class ScopeGuard
{
    TCall _func;
public:
    explicit ScopeGuard(const TCall& fn)
        : _func(fn)
    {
    }
    explicit ScopeGuard(TCall&& fn)
        : _func(std::move(fn))
    {
    }
    ~ScopeGuard()
    {
        _func();
    }
};

template<class TCall, bool executeOnException>
class ScopeGuardForNewException
{
    TCall _func;
    UncaughtExceptionCounter _ec;
public:
    explicit ScopeGuardForNewException(const TCall& fn)
        : _func(fn)
    {
    }
    explicit ScopeGuardForNewException(TCall&& fn)
        : _func(std::move(fn))
    {
    }
    ~ScopeGuardForNewException() noexcept(executeOnException)
    {
        if (executeOnException == _ec.newUncaughtException())
        {
            _func();
        }
    }
};

enum class ScopeGuardOnExit {};

template<class TCall>
ScopeGuard<TCall> operator+(ScopeGuardOnExit, TCall&& fn)
{
    return ScopeGuard<TCall>(std::forward<TCall>(fn));
}

enum class ScopeGuardOnFail {};

template<class TCall>
ScopeGuardForNewException<std::decay_t<TCall>, true>
operator+(ScopeGuardOnFail, TCall&& fn)
{
    return ScopeGuardForNewException<std::decay_t<TCall>, true>(std::forward<TCall>(fn));
}

enum class ScopeGuardOnSuccess {};

template<class TCall>
ScopeGuardForNewException<std::decay_t<TCall>, false>
operator+(ScopeGuardOnSuccess, TCall&& fn)
{
    return ScopeGuardForNewException<std::decay_t<TCall>, false>(std::forward<TCall>(fn));
}

} //namespace detail

#define MQ_CONCAT_IMPL(s1, s2) s1##s2
#define MQ_CONCAT(s1, s2) MQ_CONCAT_IMPL(s1, s2)

#ifdef __COUNTER__
#define MQ_ANONYMOUS_VARIABLE(str) \
    MQ_CONCAT(str, __COUNTER__)
#else
#define MQ_ANONYMOUS_VARIABLE(str) \
    MQ_CONCAT(str, __LINE__)
#endif

//execute on scope exit, must never throw
#define MQ_SCOPE_EXIT \
    auto MQ_ANONYMOUS_VARIABLE(MQ_SCOPE_EXIT_STATE) \
    = ::mq::detail::ScopeGuardOnExit() + [&]() noexcept

//execute on exception is raised, must never throw
#define MQ_SCOPE_FAIL \
    auto MQ_ANONYMOUS_VARIABLE(MQ_SCOPE_FAIL_STATE) \
    = ::mq::detail::ScopeGuardOnFail() + [&]() noexcept

//execute on scope exit with no exception, may throw
#define MQ_SCOPE_SUCCESS \
    auto MQ_ANONYMOUS_VARIABLE(MQ_SCOPE_SUCCESS_STATE) \
    = ::mq::detail::ScopeGuardOnSuccess() + [&]()

}
