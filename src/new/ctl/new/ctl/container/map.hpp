#ifndef CTL_CONTAINER_MAP_H
#define CTL_CONTAINER_MAP_H

#include "list.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../algorithm/hash.hpp"
#include "pair.hpp"

template<class TKey, class TValue, template<class TKey, class TValue> class TPair = Pair>
struct Collected {
	typedef Typed<TKey>			Key;
	typedef Typed<TValue>		Value;

	typedef TKey				KeyType;
	typedef TValue				ValueType;
	typedef TPair<TKey, TValue>	PairType;
};

template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = KeyValuePair<TKey, TValue>>
struct OrderedMap:
	List<TPair<TKey, TValue>, TIndex>,
	Collected<TKey, TValue, TPair>,
	SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>,
	Derived<List<TPair<TKey, TValue>, TIndex>>,

requires (
	Type::Comparable::Threeway<TKey, TKey>
&&	PairType<TPair>
) {
public:
	using Derived			= Derived<List<TPair<TKey, TValue>, TIndex>>;
	using Collected			= Collected<TKey, TValue, TPair>;
	using SelfIdentified	= SelfIdentified<OrderedMap<TKey, TValue, TIndex, TPair>>;

	using BaseType = typename Derived::Bases::FirstType;

	using
		typename Collected::KeyType,
		typename Collected::ValueType,
		typename Collected::PairType
	;

	using SelfIdentified::SelfType;

	using BaseType::BaseType;

	constexpr ValueType at(KeyType const& key) const {
		if (empty()) return ValueType();
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return ValueType();
	}

	constexpr ReferenceType	operator[](KeyType const& index) {
		if (empty()) return pushBack({key}).back().value;
		PairType* start = cbegin();
		while(start != cend()) {
			if (start->key == key) return start->value;
			++start;
		}
		return pushBack({key}).back().value;
	}

	constexpr ValueType operator[](KeyType const& index) const	{return at(index);}

	constexpr bool contains(KeyType const& key) const {
		if (empty()) return false;
		for(ValueType& e: (*this))
			if (e.key == key) return true;
		return false;
	}

private:
};

/*
template<class TKey, class TValue, Type::Integer TIndex = usize, class TPair = Pair<TKey, TValue>, class THasher = Hasher>
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
*/

#endif // CTL_CONTAINER_MAP_H
