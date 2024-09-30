#ifndef CTL_CONTAINER_DICTIONARY_H
#define CTL_CONTAINER_DICTIONARY_H

#include "../namespace.hpp"
#include "map.hpp"
#include "string.hpp"

CTL_NAMESPACE_BEGIN

template<class TValue, Type::Integer TIndex = usize, template<class TKey, class TValue> class TPair = Pair>>
using OrderedDictionary	= OrderedMap<String, TValue, TIndex, TPair>;

template<class TValue, Type::Integer TIndex = usize, template<class TKey, class TValue> class TPair = Pair>>
using SimpleDictionary	= SimpleMap<String, TValue, TIndex, TPair>;

template<class TValue, Type::Integer TIndex = usize, template<class TKey, class TValue> class TPair = Pair>
using Dictionary		= SimpleDictionary<TValue, TIndex, TPair>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_DICTIONARY_H
