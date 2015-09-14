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
class SmartPointer
{
private:
	using DeleterType = void(*)(T*);

	mutable volatile size_t* _Count;
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
	SmartPointer()
		: _Count(nullptr)
		, _Object(nullptr)
		, _Deleter(nullptr)
	{}

	SmartPointer(T* pointer, DeleterType deleter = PointerBackend<T>::DefaultDeleter)
		: _Count(new size_t(1))
		, _Object(pointer)
		, _Deleter(deleter)
	{}

	SmartPointer(const SmartPointer<T>& pointer)
		: _Count(pointer._Count)
		, _Object(pointer._Object)
		, _Deleter(pointer._Deleter)
	{
		CountInc();
	}

	SmartPointer(SmartPointer<T>&& pointer)
		: _Count(pointer._Count)
		, _Object(pointer._Object)
		, _Deleter(pointer._Deleter)
	{
		pointer._Count = nullptr;
		pointer._Object = nullptr;
		pointer._Deleter = nullptr;
	}
	SmartPointer::~SmartPointer()
	{
		CountDec();
	}
	SmartPointer& operator=(const SmartPointer<T>& pointer)
	{
		if (this != &pointer)
		{
			CountDec();
			_Object = pointer._Object;
			_Count = pointer._Count;
			_Deleter = pointer._Deleter;
			CountInc();
		}
		return *this;
	}
	SmartPointer& operator=(SmartPointer<T>&& pointer)
	{
		if (this != &pointer)
		{
			CountDec();
			_Object = pointer._Object;
			_Count = pointer._Count;
			_Deleter = pointer._Deleter;

			pointer._Object = nullptr;
			pointer._Count = nullptr;
			pointer._Deleter = nullptr;
		}
		return *this;
	}
	bool operator==(const T* pointer)const { return _Object == pointer; }
	bool operator!=(const T* pointer)const { return _Object != pointer; }
	bool operator>(const T* pointer)const { return _Object > pointer; }
	bool operator>=(const T* pointer)const { return _Object >= pointer; }
	bool operator<(const T* pointer)const { return _Object < pointer; }
	bool operator<=(const T* pointer)const { return _Object <= pointer; }

	bool operator==(const SmartPointer<T>& pointer)const { return _Object == pointer._Object; }
	bool operator!=(const SmartPointer<T>& pointer)const { return _Object != pointer._Object; }
	bool operator>(const SmartPointer<T>& pointer)const { return _Object > pointer._Object; }
	bool operator>=(const SmartPointer<T>& pointer)const { return _Object >= pointer._Object; }
	bool operator<(const SmartPointer<T>& pointer)const { return _Object < pointer._Object; }
	bool operator<=(const SmartPointer<T>& pointer)const { return _Object <= pointer._Object; }

	operator bool() const
	{
		return _Object != nullptr;
	}

	T* Get() const
	{
		return _Object;
	}

	T* operator->()const
	{
		return _Object;
	}
};
#endif // !POINTER_H
