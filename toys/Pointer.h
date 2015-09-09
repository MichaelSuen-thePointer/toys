#pragma once
#ifndef POINTER_H
#define POINTER_H

template <typename T>
class PointerBackend
{
private:
	inline static void DefaultDeleter(T* pointer)
	{
		delete pointer;
	}
};

template <typename T>
class ConvenientPointer
{
private:
	using DeleterType = void(*)(T*);

	volatile size_t* _Count;
	T* _Object;
	DeleterType _Deleter;

	void CountInc()
	{
		if (_Count)
		{
			(*_Count)++;
		}
	}

	void CountDec()
	{
		if (_Count)
		{
			(*_Count)--;
			if (*_Count == 0)
			{
				_Deleter(_Object);
				delete _Count;
			}
		}
	}
public:
	ConvenientPointer()
		: _Count(nullptr)
		, _Object(nullptr)
		, _Deleter(nullptr)
	{}

	ConvenientPointer(T* pointer)
		: _Count(new size_t(1))
		, _Object(pointer)
		, _Deleter(PointerBackend<T>::DefaultDeleter)
	{}

	ConvenientPointer(const ConvenientPointer<T>& pointer)
		: _Count(pointer._Count)
		, _Object(pointer._Object)
		, _Deleter(pointer._Deleter)
	{
		CountInc();
	}

	ConvenientPointer(ConvenientPointer<T>&& pointer)
		: _Count(pointer._Count)
		, _Object(pointer._Object)
		, _Deleter(pointer._Deleter)
	{
		pointer._Count = nullptr;
		pointer._Object = nullptr;
		pointer._Deleter = nullptr;
	}


};
#endif // !POINTER_H
