#pragma once

#include "Base.h"
#include "Template.h"
#ifndef NUMBERS_H
#define NUMBERS_H

namespace pl
{
namespace number
{
struct Zero
{
};

template <typename TPrev>
struct PlusOne
{
	using Type = typename PlusOne<TPrev>;
	using Prev = typename TPrev;
};

template <typename T>
struct ShowValue
{
	enum : unsigned { Value = ShowValue<T::Prev>::Value + 1 };
};

template <>
struct ShowValue<Zero>
{
	enum : unsigned { Value = 0 };
};

template <unsigned Num>
struct Integer
{
	using Type = typename PlusOne<typename Integer<Num - 1>::Type>::Type;
};

template <>
struct Integer<0>
{
	using Type = Zero;
};

template <unsigned Num1, unsigned Num2>
struct AddNum
{
	using Type = typename Integer<Num1 + Num2>::Type;
};

template <typename NumberT1, typename NumberT2>
struct Add
{
	using Type = typename AddNum<ShowValue<typename NumberT1::Type>::Value, ShowValue<typename NumberT2::Type>::Value>::Type;
};



} //namespace number




template<typename TEnum, typename TInteger>
class EnumMapper
{
public:
	using TTo = typename EnableIf<TypeEq<typename EnumBase<TEnum>::Type, TInteger>::Value, TInteger>::Type;
};

template<typename TEnum, typename TInteger, TEnum V>
class StaticMapper : public EnumMapper<TEnum, TInteger>
{
public:
	constexpr static TTo To = static_cast<TTo>(V);
};


} //namespace pl


#endif // !NUMBERS_H
