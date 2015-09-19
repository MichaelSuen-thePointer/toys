#pragma once
#ifndef CPS_H
#define CPS_H

#include <utility>

#include <functional>
template<typename TRet, typename... TArgs>
using Func = std::function<TRet(TArgs...)>;

void TestFunc(Func<void(Func<void()>)> func1, Func<void()> func2)
{
}

void Conditional(bool Condition, std::function<void(std::function<void()>)> True, std::function<void(std::function<void()>)> False, Func<void(void)> Continuation)
{
	if (Condition)
	{
		True(Continuation);
	}
	else
	{
		False(Continuation);
	}
}


#endif // !CPS_H
