#pragma once

#include "SetExtension.h"

namespace pl
{
using namespace setext;

using Relation = std::set<char>;
using FunctionalDependency = std::pair<std::set<char>, std::set<char>>;
using DependencySet = std::set<std::pair<std::set<char>, std::set<char>>>;

std::set<char> closure(const DependencySet & dependencySet, const std::set<char>& property);

DependencySet closure(const Relation & rela, const DependencySet & dset);

FunctionalDependency remove_extraneous_property(const DependencySet & dset, const FunctionalDependency & dep);

DependencySet canonical_cover(const DependencySet & dependencySet);

bool is_BCNF(const Relation & rela, const DependencySet & dset, const FunctionalDependency & dep);

bool is_BCNF(const Relation & rela, const DependencySet & dset);

bool is_in_closure_of_schema(const DependencySet & dset, const FunctionalDependency & dep);

std::set<Relation> BCNF_decomposition(const Relation & rela, const DependencySet & dset);



}