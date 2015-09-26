#pragma once
#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <type_traits>
#include <utility>

int a = std::forward<int>(10);

namespace pl
{

struct EmptyType
{};

template<bool VTest, typename T = void>
struct EnableIf
{};

template<typename T>
struct EnableIf<true, T>
{
    using Type = typename T;
};

template<typename T1, typename T2>
struct TypeEq: FalseType
{};

template<typename T>
struct TypeEq<T, T>: TrueType
{};

template<typename TEnum>
struct EnumBase
{
    using Type = __underlying_type(TEnum);
};


template<typename T>
struct RemoveConst
{
    using Type = T;
};

template<typename T>
struct RemoveConst<const T>
{
    using Type = T;
};

template<class T>
struct RemoveReference
{
    using Type = T;
};

template<class T>
struct RemoveReference<T&>
{
    using Type = T;
};

template<class T>
struct RemoveReference<T&&>
{
    using Type = T;
};

template<class T>
struct IsLvalueReference: FalseType
{};

template<class T>
struct IsLvalueReference<T&> : TrueType
{};


template<class T> inline constexpr 
typename RemoveReference<T>::Type&& RvalueCast(T&& value) noexcept
{
    return (static_cast<typename RemoveReference<T>::Type&&>(value));
}

template<class T> inline constexpr
T&& PerfectForward(typename RemoveReference<T>::Type& value) noexcept
{
    return static_cast<T&&>(value);
}

template<class T> inline constexpr
T&& PerfectForward(typename RemoveReference<T>::Type&& value) noexcept
{
    static_assert(!IsLvalueReference<T>::Value, "bad forward call");
    return (static_cast<T&&>(value));
}

template<typename TPOD, TPOD V>
struct PODType
{
    static const TPOD Value = V;
    using Type = TPOD;
    using Self = PODType<TPOD, V>;
};


template<int VInt>
using Int = PODType<int, VInt>;

template<bool VBool>
using Bool = PODType<bool, VBool>;

template<char VChar>
using Char = PODType<char, VChar>;

template<typename TTest, typename TTrue, typename TFalse>
struct If;

template<typename TTrue, typename TFalse>
struct If<Bool<true>, TTrue, TFalse>
{
    using Result = TTrue;
};

template<typename TTrue, typename TFalse>
struct If<Bool<false>, TTrue, TFalse>
{
    using Result = TFalse;
};

struct POD
{};
struct NotPOD
{};

template<typename T>
struct IsPOD: Bool<__is_pod(T)>
{
    using Result = typename If<Self, POD, NotPOD>::Result;
};

using TrueType = Bool<true>;
using FalseType = Bool<false>;

#define TN typename
#define TT template
#define SS struct

#define DEFINE_BIN_META_OPERATION_HEAD(Name)	\
TT<TN T1, TN T2>								\
SS Name

#define DEFINE_BIN_META_OPERATION_BODY(Name, TType, VType, RType, op)	\
TT<VType V1, VType V2>													\
SS Name<TType<V1>, TType<V2>>											\
{																		\
    using Result = TN RType<(V1 op V2)>;								\
}

#define DEFINE_UNA_META_OPERATION_HEAD(Name)	\
TT<TN T1>										\
SS Name

#define DEFINE_UNA_META_OPERATION_BODY(Name, TType, VType, RType, op)	\
TT<VType V1>															\
SS Name<TType<V1>>														\
{																		\
    using Result = TN RType<(op V1)>;									\
}

DEFINE_BIN_META_OPERATION_HEAD(Add);
DEFINE_BIN_META_OPERATION_BODY(Add, Int, int, Int, +);
DEFINE_BIN_META_OPERATION_BODY(Add, Char, char, Char, +);
DEFINE_BIN_META_OPERATION_HEAD(Sub);
DEFINE_BIN_META_OPERATION_BODY(Sub, Int, int, Int, -);
DEFINE_BIN_META_OPERATION_BODY(Sub, Char, char, Char, -);
DEFINE_BIN_META_OPERATION_HEAD(Mul);
DEFINE_BIN_META_OPERATION_BODY(Mul, Int, int, Int, *);
DEFINE_BIN_META_OPERATION_BODY(Mul, Char, char, Char, *);
DEFINE_BIN_META_OPERATION_HEAD(Div);
DEFINE_BIN_META_OPERATION_BODY(Div, Int, int, Int, / );
DEFINE_BIN_META_OPERATION_BODY(Div, Char, char, Char, / );
DEFINE_BIN_META_OPERATION_HEAD(Mod);
DEFINE_BIN_META_OPERATION_BODY(Mod, Int, int, Int, %);
DEFINE_BIN_META_OPERATION_BODY(Mod, Char, char, Char, %);
DEFINE_BIN_META_OPERATION_HEAD(And);
DEFINE_BIN_META_OPERATION_BODY(And, Bool, bool, Bool, &&);
DEFINE_BIN_META_OPERATION_HEAD(Or);
DEFINE_BIN_META_OPERATION_BODY(Or, Bool, bool, Bool, || );
DEFINE_BIN_META_OPERATION_HEAD(Xor);
DEFINE_BIN_META_OPERATION_BODY(Xor, Bool, bool, Bool, ^);
DEFINE_BIN_META_OPERATION_HEAD(Eq);
DEFINE_BIN_META_OPERATION_BODY(Eq, Bool, bool, Bool, == );
DEFINE_BIN_META_OPERATION_BODY(Eq, Int, int, Bool, == );
DEFINE_BIN_META_OPERATION_BODY(Eq, Char, char, Bool, == );
DEFINE_BIN_META_OPERATION_HEAD(Neq);
DEFINE_BIN_META_OPERATION_BODY(Neq, Bool, bool, Bool, != );
DEFINE_BIN_META_OPERATION_BODY(Neq, Int, int, Bool, != );
DEFINE_BIN_META_OPERATION_BODY(Neq, Char, char, Bool, != );
DEFINE_BIN_META_OPERATION_HEAD(Gt);
DEFINE_BIN_META_OPERATION_BODY(Gt, Int, int, Bool, > );
DEFINE_BIN_META_OPERATION_BODY(Gt, Char, char, Bool, > );
DEFINE_BIN_META_OPERATION_HEAD(Lt);
DEFINE_BIN_META_OPERATION_BODY(Lt, Int, int, Bool, < );
DEFINE_BIN_META_OPERATION_BODY(Lt, Char, char, Bool, < );
DEFINE_BIN_META_OPERATION_HEAD(Ge);
DEFINE_BIN_META_OPERATION_BODY(Ge, Int, int, Bool, >= );
DEFINE_BIN_META_OPERATION_BODY(Ge, Char, char, Bool, >= );
DEFINE_BIN_META_OPERATION_HEAD(Le);
DEFINE_BIN_META_OPERATION_BODY(Le, Int, int, Bool, <= );
DEFINE_BIN_META_OPERATION_BODY(Le, Char, char, Bool, <= );
DEFINE_UNA_META_OPERATION_HEAD(Neg);
DEFINE_UNA_META_OPERATION_BODY(Neg, Int, int, Int, -);
DEFINE_UNA_META_OPERATION_BODY(Neg, Char, char, Char, -);
DEFINE_UNA_META_OPERATION_HEAD(Not);
DEFINE_UNA_META_OPERATION_BODY(Not, Bool, bool, Bool, !);

#undef TT
#undef TN
#undef SS

template<typename TTimes, template<typename TResult> typename TProc, typename TDefVal>
struct FOR_N;

template<template<typename TResult> typename TProc, typename TDefVal, int VTimes>
struct FOR_N<Int<VTimes>, TProc, TDefVal>
{
    using Result = typename TProc<
        typename FOR_N<
            Int<VTimes - 1>,
            TProc,
            TDefVal
        >::Result
    >::Result;
};

template<template<typename TResult> typename TProc, typename TDefVal>
struct FOR_N<Int<0>, TProc, TDefVal>
{
    using Result = typename TDefVal;
};

#if defined SIMPLIFIED_TML

#define IF If<
#define THEN ,
#define ELSE ,
#define NOELSE , void
#define NOTHEN , void
#define ENDIF >::Result
#define ELIF ELSE IF

#endif

template<typename T>
struct ListGet;
template<typename... TArgs>
struct List;

template<typename T, typename... TArgs>
struct ListGet<List<T, TArgs...>>
{
    using First = typename T;
    using Rest = List<TArgs...>;
};

template<typename... TArgs>
struct List: ListGet<List<TArgs...>>
{
    using Base = ListGet<List<TArgs...>>;
    using Self = List<TArgs...>;
};

template<typename T>
struct ListIsEmpty;

template<typename T, typename... TArgs>
struct ListIsEmpty<List<T, TArgs...>>
{
    using Result = Bool<false>;
};
template<>
struct ListIsEmpty<List<>>
{
    using Result = Bool<true>;
};

template<typename A, template<typename... Args> typename B>
struct Change;

template<template<typename... Args>typename A, template<typename... Args>typename B, typename... Args>
struct Change<A<Args...>, B>
{
    using Result = B<Args...>;
};

template<typename T1, typename T2>
struct ListConnect;

template<typename... TArgs1, typename... TArgs2>
struct ListConnect<List<TArgs1...>, List<TArgs2...>>
{
    using Result = typename List<TArgs1..., TArgs2...>;
};
template<typename T>
struct ListReverse;

template<typename... TArgs>
struct ListReverse<List<TArgs...>>
{
    using Param = typename List<TArgs...>;
    using Result = typename ListConnect<
        typename ListReverse<
            typename Param::Rest
        >::Result,
        typename List<typename Param::First>
    >::Result;
};

template<typename T>
struct ListReverse<List<T>>
{
    using Result = typename List<T>;
};

template<template<typename ST> typename TApplicative, typename T1>
struct ListMap;

template<template<typename ST> typename TApplicative, typename... TArgs>
struct ListMap<TApplicative, List<TArgs...>>
{
    using Param = typename List<TArgs...>;
    using Result = typename ListConnect<
        List<
            typename TApplicative<
                typename Param::First
            >::Result
        >,
        typename ListMap<
            TApplicative,
            typename Param::Rest
        >::Result
    >::Result;
};

template<template<typename ST> typename TApplicative, typename TLast>
struct ListMap<TApplicative, List<TLast>>
{
    using Param = typename List<TLast>;
    using Result = typename List<typename TApplicative<TLast>::Result>;
};

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename T>
struct ListFoldLeft;

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename... TArgs>
struct ListFoldLeft<TVal, TBinFunc, List<TArgs...>>
{
    using Param = List<TArgs...>;
    using Result = typename ListFoldLeft<
        typename TBinFunc<
            TVal,
            typename Param::First
        >::Result,
        TBinFunc,
        typename Param::Rest		
    >::Result;
};

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename TLast>
struct ListFoldLeft<TVal, TBinFunc, List<TLast>>
{
    using Param = List<TLast>;
    using Result = typename TBinFunc<TVal, typename Param::First>::Result;
};

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename T>
struct ListScanLeft;

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename... TArgs>
struct ListScanLeft<TVal, TBinFunc, List<TArgs...>>
{
    using Param = List<TArgs...>;
    using Value = typename TBinFunc<
        TVal,
        typename Param::First
    >::Result;
    using Result = typename ListConnect<
        List<Value>,
        typename ListScanLeft<
            Value,
            TBinFunc,
            typename Param::Rest
        >::Result
    >::Result;
};

template<typename TVal, template<typename TArg1, typename TArg2> typename TBinFunc, typename TLast>
struct ListScanLeft<TVal, TBinFunc, List<TLast>>
{
    using Param = List<TLast>;
    using Value = typename TBinFunc<
        TVal,
        typename Param::First
    >::Result;
    using Result = List<Value>;
};

template<template<typename TArg> typename TFilter, typename T>
struct ListFilter;

template<template<typename TArg> typename TFilter, typename... TArgs>
struct ListFilter<TFilter, List<TArgs...>>
{
    using Param = List<TArgs...>;
    using Result = typename ListConnect<
        typename If<
            typename TFilter<typename Param::First>::Result,
            List<typename Param::First>,
            List<>
        >::Result,
        typename ListFilter<
            TFilter, 
            typename Param::Rest
        >::Result
    >::Result;
};

template<template<typename TArg> typename TFilter, typename TLast>
struct ListFilter<TFilter, List<TLast>>
{
    using Param = List<TLast>;
    using Result = typename If<
        typename TFilter<typename Param::First>::Result,
        List<typename Param::First>,
        List<>
    >::Result;
};

template<typename T>
struct MapNone
{
    using Result = T;
};

template<typename T>
struct FilterNone
{
    using Result = Bool<true>;
};

template<template<typename TVal> typename TProcToVal, template<typename TVal> typename TFilterProc, typename... TSeq>
struct ListGeneration;

template<template<typename TVal> typename TProcToVal, template<typename TVal> typename TFilterProc, typename... TSeq>
struct ListGeneration<TProcToVal, TFilterProc, List<TSeq...>>
{
    using FilterResult = typename ListFilter<TFilterProc, List<TSeq...>>::Result;
    using Result = typename ListMap<TProcToVal, FilterResult>::Result;
};

template<typename TType, TType VBegin, TType VEnd, TType VCurr = VBegin>
struct Range
{
    using Result = typename ListConnect<
        List<PODType<TType, VCurr>>, 
        typename If<
            Bool<VCurr < VEnd>, 
            typename Range<TType, VBegin, VEnd, VCurr + 1>::Result,
            List<>
        >::Result
    >::Result;
};

template<typename T, T V, typename... TRest>
void PrintList(List<PODType<T, V>, TRest...>)
{
    std::cout << V << ' ';
    PrintList(List<TRest...>());
}

template<typename T, T V>
void PrintList(List<PODType<T, V>>)
{
    std::cout << V;
}


/*SFINAE*/

//template<typename T>
//struct IsClass
//{
//private:
//	template<typename TClass> static Bool<true> test(int TClass::*) {}
//	template<typename TClass> static Bool<false> test(...) {}
//public:
//	using Result = decltype(test<T>(0));
//};

#define DEFINE_TYPE_SFINAE_CLASS(Name, ClassTemplateName, AcceptTypePtr)			\
template<typename T>																\
struct Name																			\
{																					\
    template<typename ClassTemplateName> static Bool<true> test(AcceptTypePtr){}	\
    template<typename ClassTemplateName> static Bool<false> test(...) {}			\
    using Result = decltype(test<T>(nullptr));										\
}

DEFINE_TYPE_SFINAE_CLASS(IsClass, TClass, int TClass::*);


}

#endif // !TEMPLATE_H
