#pragma once

#include <set>
#include <algorithm>
#include <iterator>
#include <boost/dynamic_bitset.hpp>
namespace pl
{
namespace setext
{

template<typename TVal>
std::set<TVal> operator+(const std::set<TVal>& set, const TVal& value)
{
	std::set<TVal> result(set);

	result.insert(value);

	return result;
}

template<typename TVal>
std::set<TVal> operator+(const std::set<TVal>& lhs, const std::set<TVal>& rhs)
{
	std::set<TVal> result(lhs);
	result.insert(rhs.begin(), rhs.end());
	return result;
}

template<typename TVal>
std::set<TVal> operator-(const std::set<TVal>& set, const TVal& value)
{
	std::set<TVal> result(set);

	result.erase(value);

	return result;
}

template<typename TVal>
std::set<TVal> operator-(const std::set<TVal>& lhs, const std::set<TVal>& rhs)
{
	std::vector<TVal> result;

	std::set_difference(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::back_inserter(result));

	return std::set<TVal>(std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
}

template<typename TVal>
std::set<TVal> operator^(const std::set<TVal>& lhs, const std::set<TVal>& rhs)
{
	std::vector<TVal> result;

	std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::back_inserter(result));

	return std::set<TVal>(std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
}

template<typename TVal>
bool includes(const std::set<TVal>& lhs, const std::set<TVal>& rhs)
{
	return std::includes(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}


template<typename TVal, typename TFunc>
void remove_all_if(std::set<TVal>& cset, TFunc& func)
{
	auto iter = cset.begin();
	while (iter != cset.end())
	{
		if (func(*iter))
		{
			cset.erase(iter);
			iter = cset.begin();
		}
		else
		{
			iter++;
		}
	}
}

template<class TCont>
class subset_generator
{
private:
	bool _valid;
	const TCont* _container;
	boost::dynamic_bitset<> _choose;
public:
	using self = subset_generator<TCont>;
	using value_type = typename TCont::value_type;
	using container_type = TCont;
	subset_generator()
		: _valid(false)
		, _container(nullptr)
		, _choose()
	{
	}
	subset_generator(const TCont& container)
		: _valid(false)
		, _container(std::addressof(container))
		, _choose(container.size() + 1)
	{
	}
	bool operator==(const self& rhs)
	{
		return _container == rhs._container && _choose == rhs._choose;
	}
	bool operator!=(const self& rhs)
	{
		return _container != rhs._container || _choose != rhs._choose;
	}
	self& operator++()
	{
		bool carry = true;
		for (size_t i = 0; carry; i++)
		{
			bool newCarry = _choose[i] & carry;
			_choose[i] = _choose[i] ^ carry;
			carry = newCarry;
		}
		if (_choose[_choose.size() - 1])
		{
			_valid = false;
		}
		return *this;
	}

	self operator++(int)
	{
		self copy(*this);
		bool carry = true;
		for (size_t i = 0; carry; i++)
		{
			bool newCarry = _choose[i] & carry;
			_choose[i] = _choose[i] ^ carry;
			carry = newCarry;
		}
		if (_choose[_choose.size() - 1])
		{
			_valid = false;
		}
		return copy;
	}

	TCont operator*()
	{
		assert(valid());
		TCont result;

		auto iter = _container->begin();
		for (size_t i = 0; i != _container->size(); i++, iter++)
		{
			if (_choose[i])
			{
				result.insert(*iter);
			}
		}

		return result;
	}
	bool valid()
	{
		return _valid;
	}
	void reset(const TCont& container)
	{
		_valid = true;
		_container = std::addressof(container);
		_choose.reset();
	}
	void reset()
	{
		_valid = true;
		_choose.reset();
	}
	operator bool()
	{
		return _valid;
	}
};

}
}