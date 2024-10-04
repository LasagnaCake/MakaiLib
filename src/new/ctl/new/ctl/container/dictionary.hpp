#ifndef CTL_CONTAINER_DICTIONARY_H
#define CTL_CONTAINER_DICTIONARY_H

#include "../namespace.hpp"
#include "map.hpp"
#include "string.hpp"

CTL_NAMESPACE_BEGIN

template<class TValue, Type::Integer TIndex = usize>
using OrderedDictionary	= OrderedMap<String, TValue, TIndex>;

template<class TValue, Type::Integer TIndex = usize>
using SimpleDictionary	= SimpleMap<String, TValue, TIndex>;

template<class TValue, Type::Integer TIndex = usize>
using Dictionary		= SimpleDictionary<TValue, TIndex>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_DICTIONARY_H
