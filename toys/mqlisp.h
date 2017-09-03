#pragma once
#include <type_traits>
#include <tuple>
#include <iostream>
namespace mq::lisp
{

namespace detail
{

template<class Derived>
struct term
{
    constexpr decltype(auto) eval()
    {
        return Derived{}.eval();
    }

    constexpr decltype(auto) operator+();
};

template<auto...>
struct atom;

template<char... chars>
struct atom<chars...> : term<atom<chars...>>
{
};

template<class T1, class T2>
struct cell : term<cell<T1, T2>>
{
};

template<class T1, class T2>
constexpr decltype(auto) mkcell(T1, T2)
{
    return cell<T1, T2>{};
}

template<class...>
struct list;

template<class T1, class TR>
constexpr decltype(auto) mklist(T1 a, TR r)
{
    return list<T1, TR>{};
}

template<class T>
constexpr decltype(auto) aslist(term<T> a)
{
    return mklist(a, list<>{});
}

template<class... Ts>
constexpr decltype(auto) aslist(list<Ts...> a)
{
    return a;
}

template<class Derived>
constexpr decltype(auto) term<Derived>::operator+()
{
    return aslist(Derived{});
}

template<class TAtom, class TU1>
struct receval_helper : term<receval_helper<TAtom, TU1>>
{
    template<class... Ts>
    constexpr decltype(auto) operator()(Ts... s)
    {
        return TAtom{}(TU1{}, s...);
    }
};

template<class T1, class TR>
struct list<T1, TR> : term<list<T1, TR>>
{
    constexpr decltype(auto) eval()
    {
        return receval(T1{}, TR{});
    }

    constexpr decltype(auto) car()
    {
        return T1{};
    }

    constexpr decltype(auto) cdr()
    {
        return TR{};
    }
private:
    template<class Atom, class T>
    constexpr static decltype(auto) receval(term<Atom>, list<T, list<>>)
    {
        return Atom{}(T{});
    }
    template<class Atom, class U1, class UR>
    constexpr static decltype(auto) receval(term<Atom>, list<U1, UR>)
    {
        return receval(receval_helper<Atom, U1>{}, UR{});
    }
};

struct nil : term<nil>
{
    constexpr decltype(auto) eval()
    {
        return nil{};
    }
};

template<>
struct list<> : term<list<>>
{
    constexpr decltype(auto) eval()
    {
        return nil{};
    }
};

struct t : term<t>
{
    constexpr static decltype(auto) eval()
    {
        return t{};
    }
};

template<auto... vs>
constexpr t operator==(atom<vs...>, atom<vs...>)
{
    return {};
}

template<auto... vs1, auto... vs2>
constexpr nil operator==(atom<vs1...>, atom<vs2...>)
{
    return {};
}

template<auto... vs>
constexpr nil operator!=(atom<vs...>, atom<vs...>)
{
    return {};
}

template<auto... vs1, auto... vs2>
constexpr t operator!=(atom<vs1...>, atom<vs2...>)
{
    return {};
}

template<class T>
struct quote : term<quote<T>>
{
    constexpr static decltype(auto) eval()
    {
        return T{};
    }
};

template<>
struct atom<'q', 'u', 'o', 't', 'e'> : term<atom<'q', 'u', 'o', 't', 'e'>>
{
    template<class T>
    constexpr decltype(auto) operator()(T v)
    {
        return quote<T>{};
    }
};

template<>
struct atom<'a', 't', 'o', 'm'> : term<atom<'a', 't', 'o', 'm'>>
{
    constexpr decltype(auto) operator()(detail::list<>)
    {
        return t{};
    }

    template<char... Chars>
    constexpr decltype(auto) operator()(detail::atom<Chars...>)
    {
        return t{};
    }

    template<class T>
    decltype(auto) operator()(detail::term<T>)
    {
        return nil{};
    }
};

template<>
struct atom<'e', 'q'> : term<atom<'e', 'q'>>
{
    template<class T1, class T2>
    constexpr decltype(auto) operator()(detail::term<T1> a, detail::term<T2> b)
    {
        return a.eval() == b.eval();
    }
};

template<>
struct atom<'c', 'a', 'r'> : term<atom<'c', 'a', 'r'>>
{
    template<class T1, class TR>
    constexpr decltype(auto) operator()(detail::list<T1, TR> a)
    {
        return a.eval().car();
    }
};

template<>
struct atom<'c', 'd', 'r'> : term<atom<'c', 'd', 'r'>>
{
    template<class T1, class TR>
    constexpr decltype(auto) operator()(detail::list<T1, TR> a)
    {
        return a.eval().cdr();
    }
};

template<>
struct atom<'c', 'o', 'n', 's'> : term<atom<'c', 'o', 'n', 's'>>
{
    template<class T, class U>
    constexpr decltype(auto) operator()(detail::term<T> a, detail::term<U> b)
    {
        return mkcell(a.eval(), b.eval());
    }

    template<class T, class U1, class U2>
    constexpr decltype(auto) operator()(detail::term<T> a, detail::list<U1, U2> b)
    {
        return mklist(a.eval(), b.eval());
    }

    template<class T, class U1, class U2>
    constexpr decltype(auto) operator()(detail::term<T> a, detail::cell<U1, U2> b)
    {
        return mklist(a.eval(), b.eval());
    }

    template<class T>
    constexpr decltype(auto) operator()(detail::term<T> a, detail::list<> b)
    {
        return mklist(a.eval(), b.eval());
    }
};

template<int i>
struct atom<i> : term<atom<i>>
{
    constexpr decltype(auto) eval()
    {
        return atom<i>{};
    }
    constexpr static int val = i;

    constexpr decltype(auto) operator-()
    {
        return atom<-i>{};
    }
};

template<class R = void>
struct cond_reduce
{
    template<class P, class E>
    constexpr decltype(auto) operator>>(detail::list<P, E>)
    {
        return std::conditional_t<std::is_same_v<R, void> && std::is_same_v<decltype(P::eval()), detail::t>,
            decltype(E::eval()),
            R>{};
    }
    constexpr decltype(auto) result()
    {
        return R{};
    }
};

template<>
struct atom<'c', 'o', 'n', 'd'> : term<atom<'c', 'o', 'n', 'd'>>
{
    template<class... Ps, class... Es>
    constexpr decltype(auto) operator()(detail::list<Ps, Es>... v)
    {
        return (cond_reduce<void>{} >> ... >> v).result();
    }
};

template<>
struct atom<'i', 'f'> : term<atom<'i', 'f'>>
{
    template<class Cond, class True, class False>
    constexpr decltype(auto) operator()(term<Cond> v, term<True> a, term<False> b = nil{})
    {
        return cond(v.eval(), a, b);
    }
private:
    template<class True, class False>
    constexpr decltype(auto) cond(t, term<True> a, term<False>)
    {
        return a.eval();
    }
    template<class True, class False>
    constexpr decltype(auto) cond(nil, term<True>, term<False> b)
    {
        return b.eval();
    }
};

template<>
struct atom<'+'> : term<atom<'+'>>
{
    template<class... Ts>
    constexpr decltype(auto) operator()(detail::term<Ts>... v)
    {
        return atom<(0 + ... + v.eval().val)>{};
    }
};

template<>
struct atom<'-'> : term<atom<'-'>>
{
    template<class T1, class T2>
    constexpr decltype(auto) operator()(detail::term<T1> a, detail::term<T2> b)
    {
        return atom<a.eval().val - b.eval().val>{};
    }
};

template<>
struct atom<'/'> : term<atom<'/'>>
{
    template<class T1, class T2>
    constexpr decltype(auto) operator()(detail::term<T1> a, detail::term<T2> b)
    {
        return atom<a.eval().val / b.eval().val>{};
    }
};

template<class...>
struct right_asso_workaround_impl;

template<class T1, class T2>
struct right_asso_workaround_impl<T1, T2>
{
    using result = detail::list<T2, T1>;
};

template<class T1, class T2, class... Ts>
struct right_asso_workaround_impl<T1, T2, Ts...>
{
    using result = typename detail::right_asso_workaround_impl<detail::list<T2, T1>, Ts...>::result;
};

template<class... Ts> //for right association
struct right_asso_workaround : detail::right_asso_workaround_impl<detail::list<>, Ts...>::result
{
    using debug_result = typename detail::right_asso_workaround_impl<detail::list<>, Ts...>::result;
};

} //namespace detail

template<class... T1s, class T2>
decltype(auto) operator,(detail::right_asso_workaround<T1s...>, detail::term<T2>)
{
    return detail::right_asso_workaround<T2, T1s...>{};
}

template<class T1, class T2>
decltype(auto) operator,(detail::term<T1>, detail::term<T2>)
{
    return detail::right_asso_workaround<T2, T1>{};
}

detail::t t{};
detail::nil nil{};
detail::atom<'q', 'u', 'o', 't', 'e'>  quote{};
detail::atom<'a', 't', 'o', 'm'>  atom{};
detail::atom<'c', 'a', 'r'>  car{};
detail::atom<'c', 'd', 'r'>  cdr{};
detail::atom<'c', 'o', 'n', 's'>  cons{};
detail::atom<'c', 'o', 'n', 'd'> cond{};
detail::atom<'e', 'q'> eq{};

inline namespace literals
{

template <class TChar, TChar... Chars>
constexpr detail::atom<Chars...> operator"" _a()
{
    return{};
}

template<int, char...>
struct parse_int;

template<int p, char v, char...rest>
struct parse_int<p, v, rest...>
{
    constexpr static int value = parse_int<p * 10 + v - '0', rest...>::value;
};

template<int p>
struct parse_int<p>
{
    constexpr static int value = p;
};

template <char... chars>
constexpr decltype(auto) operator"" _a()
{
    return detail::atom<parse_int<0, chars...>::value>{};
}

} //namespace literals

template<class T>
constexpr decltype(auto) eval(detail::term<T> a)
{
    return a.eval();
}

} //namespace mq::lisp

