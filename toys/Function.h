#pragma once

#include "Base.h"

#ifndef FUNCTION_H
#define FUNCTION_H
#include <type_traits>

namespace pl
{
namespace callable_backend
{

template<typename TRet, typename... TArgs>
class Invoker: public Object
{
public:
	virtual TRet Invoke(TArgs&&... args) = 0;
};

template<typename TRet, typename... TArgs>
class NormalInvoker: public Invoker<TRet, TArgs...>
{
	using FunctionType = TRet(*)(TArgs...);
private:
	FunctionType _Function;
public:
	NormalInvoker(FunctionType function)
		: _Function(function)
	{}

	TRet Invoke(TArgs&&... args)
	{
		return _Function(std::forward<TArgs>(args)...));
	}
};

template<typename TClass, typename TRet, typename... TArgs>
class MemberInvoker: public Invoker<TRet, TArgs...>
{
	using FunctionType = TRet(TClass::*)(TArgs...);
private:
	TClass* _Object;
	FunctionType _Function;
public:
	MemberInvoker(TClass* object, FunctionType function)
		: _Object(object)
		, _Function(function)
	{}
	TRet Invoke(TArgs&&... args)
	{
		return (_Object->*_Function)(std::forward<TArgs>(args)...);
	}
};

template<typename TCallable, typename TRet, typename... TArgs>
class CallableInvoker: public Invoker<TRet, TArgs...>
{
	using FunctionType = TRet(*)(TArgs...);
private:
	TCallable* _Object;
public:
	CallableInvoker(TCallable* object)
		: _Object(object)
	{}
	TRet Invoke(TArgs&&... args)
	{
		return _Object(std::forward<TArgs>(args)...);
	}
};

} //namespace callable_backend
template<typename T>
class Callable
{};

template<typename TRet, typename... TArgs>
class Callable<TRet(TArgs...)>: public Object
{
	using CallableType = TRet(*)(TArgs...);
protected:
	callable_backend::Invoker<TRet, TArgs...>* _Invoker;
public:
	Callable(TRet(*function)(TArgs..))
		: _Invoker(new callable_backend::NormalInvoker<TRet, TArgs...>(function))
	{}

	template<typename TClass>
	Callable(TClass* object, TRet(TClass::*function)(TArgs...))
		: _Invoker(new callable_backend::MemberInvoker<TClass, TRet, TArgs...>(object, function))
	{}

	template<typename TCallable>
	Callable(TCallable* object)
		: _Invoker(new callable_backend::CallableInvoker<TCallable, TRet, TArgs...>(object))
	{}

	Callable(const Callable& callable)
		: _Invoker(callable._Invoker)
	{}

	Callable(Callable&& callable)
		: _Invoker(callable._Invoker)
	{
		callable._Invoker = nullptr;
	}

	TRet operator()(TArgs... args)
	{
		return _Invoker(std::forward<TArgs>(args)...);
	}

	bool operator==(const Callable& callable)
	{
		return _Invoker == callable._Invoker;
	}



};

}

#endif // !FUNCTION_H
