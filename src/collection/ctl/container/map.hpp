#ifndef CTL_CONTAINER_MAP_H
#define CTL_CONTAINER_MAP_H

#include "list.hpp"
#include "../templates.hpp"
#include "../conceptual.hpp"
#include "../algorithm/hash.hpp"
#include "pair.hpp"

template<class TKey, class TValue, Type::Integer TIndex = size_t, class TPair = Pair<TKey, TValue>>
struct OrderedMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>
requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
) {
public:
	using List<PairType>::List;

	constexpr Value::ReferenceType at(KeyType const& key) {
		if (empty()) return pushBack({key}).back().value;
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return pushBack({key}).back().value;
	}

	constexpr ValueType at(KeyType const& key) const {
		if (empty()) return ValueType();
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return ValueType();
	}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		for(ValueType& e: (*this))
			if (e.key == key) return true;
		return false;
	}

	constexpr ReferenceType	operator[](KeyType const& index)		{return at(index);}
	constexpr ValueType		operator[](KeyType const& index) const	{return at(index);}

private:
};

template<class TKey, class TValue, Type::Integer TIndex = size_t, class TPair = Pair<TKey, TValue>, class THasher = Hasher>
struct HashMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>
requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
&&	Hashable<TKey, THasher>
) {

}

#endif // CTL_CONTAINER_MAP_H
