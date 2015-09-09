#pragma once
#ifndef STRING_H
#define STRING_H

#include "Base.h"
#include <memory.h>
namespace pl
{
class AnsiString: public Object
{
private:
	char* _Buffer;
	volatile size_t* _RefCount;
	size_t _Start;
	size_t _Length;

	void CountInc()
	{
		(*_RefCount)++;
	}
	void CountDec()
	{
		(*_RefCount)--;
		if (_RefCount == 0)
		{
			delete _RefCount;
			delete[] _Buffer;
		}
	}
public:
	static size_t CStringLength(const char* String)
	{
		size_t size = 0;
		while (*String++)
		{
			size++;
		}
		return size;
	}
	static int Compare(const AnsiString& A, const AnsiString& B)
	{
		char* APtr = A._Buffer + A._Start;
		char* BPtr = B._Buffer + B._Start;
		while (*APtr && *BPtr)
		{
			int Diff = *APtr - *BPtr;
			if (Diff != 0)
			{
				return Diff;
			}
			APtr++;
			BPtr++;
		}
		return A._Length - B._Length;
	}
	static bool Equal(const AnsiString& A, const AnsiString& B)
	{
		if (A._Length == B._Length)
		{
			for (int i = 0; i < A._Length; i++)
			{
				if (A._Buffer[i] != B._Buffer[i])
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
private:
	AnsiString(const AnsiString& Concat1, const AnsiString& Concat2, int BeforeIndex, int Length)
		: _Buffer(new char[BeforeIndex + Length + 1])
		, _RefCount(new size_t(1))
		, _Start(0)
		, _Length(BeforeIndex + Length)
	{
		memcpy(_Buffer, Concat1._Buffer, BeforeIndex * sizeof(char));
		memcpy(_Buffer, Concat2._Buffer, Length * sizeof(char));
		_Buffer[_Length] = '\0';
	}
public: /*ctor*/
	AnsiString()
		: _Buffer(new char[1])
		, _RefCount(0)
		, _Start(0)
		, _Length(0)
	{
		_Buffer[0] = '\0';
	}
	AnsiString(const char* String, size_t Start, size_t Length)
		: _Buffer(new char[Length + 1])
		, _RefCount(new size_t(1))
		, _Start(Start)
		, _Length(Length)
	{
		memcpy(_Buffer, String + Start, Length * sizeof(char));
		_Buffer[_Length] = '\0';
	}
	AnsiString(const char* CString)
		: AnsiString(CString, 0, CStringLength(CString))
	{}
	AnsiString(const char* String, size_t Length)
		: AnsiString(String, 0, Length)
	{}
	AnsiString(const char* Start, const char* OffEnd)
		: AnsiString(Start, 0, OffEnd - Start)
	{}
	AnsiString(size_t Count, char Char)
		: _Buffer(new char[Count + 1])
		, _RefCount(new size_t(1))
		, _Start(0)
		, _Length(Count)
	{
		memset(_Buffer, Char, _Length * sizeof(char));
		_Buffer[_Length] = '\0';
	}
	AnsiString(const AnsiString& String, size_t Start, size_t Length)
		: _Buffer(String._Buffer)
		, _RefCount(String._RefCount)
		, _Start(Start)
		, _Length(Length)
	{
		CountInc();
	}
	AnsiString(const AnsiString& String)
		: AnsiString(String, String._Start, String._Length)
	{}
	AnsiString(AnsiString&& String)
		: _Buffer(String._Buffer)
		, _RefCount(String._RefCount)
		, _Start(String._Start)
		, _Length(String._Length)
	{
		String._Buffer = nullptr;
		String._RefCount = nullptr;
	}
	AnsiString(const AnsiString& String, size_t Start)
		: AnsiString(String, Start, String._Length - Start)
	{}
	~AnsiString()
	{
		if (_RefCount)
		{
			CountDec();
		}
	}
public:
	bool operator==(const AnsiString& String) { return Equal(*this, String); }
	bool operator!=(const AnsiString& String) { return !Equal(*this, String); }
	bool operator> (const AnsiString& String) { return Compare(*this, String) > 0; }
	bool operator>=(const AnsiString& String) { return Compare(*this, String) >= 0; }
	bool operator< (const AnsiString& String) { return Compare(*this, String) < 0; }
	bool operator<=(const AnsiString& String) { return Compare(*this, String) <= 0; }

	AnsiString operator+(const AnsiString& String)
	{
		return AnsiString(*this, String, _Length, String._Length);
	}

	const char* RawString()
	{
		return _Buffer;
	}

public:


};

}

#endif // !STRING_H
