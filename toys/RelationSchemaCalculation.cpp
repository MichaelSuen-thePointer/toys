#include "RelationSchemaCalculation.h"

namespace pl
{

std::set<char> closure(const DependencySet& dependencySet,
				  const std::set<char>& property)
{
	std::set<char> result(property);
	bool hasChange;
	do
	{
		hasChange = false;
		for (auto& dep : dependencySet)
		{
			if (includes(result.begin(), result.end(), dep.first.begin(), dep.first.end()))
			{
				if (!includes(result.begin(), result.end(), dep.second.begin(), dep.second.end()))
				{
					result.insert(dep.second.begin(), dep.second.end());
					hasChange = true;
				}
			}
		}
	} while (hasChange);

	return result;
}

DependencySet closure(const Relation& rela, const DependencySet& dset)
{
	DependencySet result;

	subset_generator<Relation> iter(rela), end;
	++iter;
	while (iter)
	{
		auto cl = closure(dset, *iter);
		subset_generator<std::set<char>> iter2(cl);
		++iter2;
		while (iter2)
		{
			result.insert(FunctionalDependency(*iter, *iter2));
			++iter2;
		}
		++iter;
	}
	return result;
}

FunctionalDependency remove_extraneous_property(const DependencySet& dset, const FunctionalDependency& dep)
{
	FunctionalDependency result(dep);
	for (auto& elm : dep.second)
	{
		DependencySet newSet = (dset - dep) + (FunctionalDependency{ dep.first, dep.second - elm });
		auto cl = closure(newSet, std::set<char>{dep.first});
		if (cl.find(elm) != cl.end())
		{
			result.second.erase(elm);
		}
	}
	for (auto& elm : dep.first)
	{
		std::set<char> gamma = dep.first - elm;
		auto cl = closure(dset, gamma);
		if (includes(cl.begin(), cl.end(), dep.second.begin(), dep.second.end()))
		{
			result.second.erase(elm);
		}
	}
	return result;
}

DependencySet canonical_cover(const DependencySet& dependencySet)
{
	DependencySet result(dependencySet);
	bool hasChange;
	do
	{
		hasChange = false;
		auto place = adjacent_find(result.begin(), result.end(), [](auto a, auto b) {
			return a.first == b.first;
		});
		if (place != result.end())
		{
			auto succ = next(place);
			std::set<char> newSet(place->second);
			newSet.insert(succ->second.begin(), succ->second.end());

			succ = next(succ);
			auto newElem = make_pair(place->first, newSet);
			result.erase(place, succ);
			result.insert(newElem);
			hasChange = true;
		}

		for (auto iter = result.begin(); iter != result.end(); iter++)
		{
			auto replaced = remove_extraneous_property(result, *iter);
			if (replaced != *iter)
			{
				hasChange = true;
				result.erase(iter);
				result.insert(replaced);
				break;
			}
		}
	} while (hasChange);

	setext::remove_all_if(result, [](auto& elm) {return elm.second.size() == 0 || elm.first.size() == 0; });

	return result;
}

bool is_BCNF(const Relation& rela, const DependencySet& dset, const FunctionalDependency& dep)
{
	auto cl = closure(dset, dep.first);
	if (rela == cl)
	{
		return true;
	}
	return false;
}

bool is_BCNF(const Relation& rela, const DependencySet& dset)
{
	for (auto& funcDep : dset)
	{
		if (includes(rela, funcDep.first) && includes(rela, funcDep.second) && !is_BCNF(rela, dset, funcDep))
		{
			return false;
		}
	}
	return true;
}

bool is_in_closure_of_schema(const DependencySet& dset, const FunctionalDependency& dep)
{
	auto cl = closure(dset, dep.first);
	return includes(cl.begin(), cl.end(), dep.second.begin(), dep.second.end());
}

std::set<Relation> BCNF_decomposition(const Relation& rela, const DependencySet& dset)
{
	std::set<Relation> result{ rela };
	bool hasDone = false;
	while (!hasDone)
	{
		for (auto& elm : result)
		{
			if (!is_BCNF(elm, dset))
			{
				for (auto& funcDep : dset)
				{
					if (includes(elm, funcDep.first) && includes(elm, funcDep.second)
						&& (funcDep.first ^ funcDep.second) == std::set<char>())
					{
						auto cl = closure(dset, funcDep.first);
						if (!includes(cl.begin(), cl.end(), elm.begin(), elm.end()))
						{
							result = (result - elm) + (elm - funcDep.second) + (funcDep.first + funcDep.second);
							goto notDone;
						}
					}
				}
			}
		}
		hasDone = true;
notDone:
		(void)0;
	}
	return result;
}

}