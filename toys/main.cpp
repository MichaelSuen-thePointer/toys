#define SIMPLIFIED_TML
#include "Template.h"
#include <iostream>
using namespace pl;

template<typename T>
struct Filter;

template<int V>
struct Filter<Int<V>>
{
	using Result = typename Ge<Int<V>, Int<2>>::Result;
};

template<typename T>
struct Mul2;

template<int V>
struct Mul2<Int<V>>
{
	using Result = Int<V * 2>;
};

template<template<typename T, T*> class Buf>
class Lexar
{
	static char str[20];
	Buf<char, Lexar<Buf>::str> buf;
};

int main()
{
	List<Int<2>, Int<3>, Int<7>, Int<1>, Int<-2>, Int<-8>, Int<0>> list;
	ListFilter<Filter, decltype(list)>::Result filtered;

	PrintList(filtered);

	//ListGeneration<Mul2, FilterNone, Range<int, 0, 63>::Result>::Result list2;
	


}