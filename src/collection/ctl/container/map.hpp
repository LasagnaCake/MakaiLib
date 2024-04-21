#ifndef CTL_CONTAINER_MAP_H
#define CTL_CONTAINER_MAP_H

#include "list.hpp"
#include "../templates.hpp"
#include "../conceptual.hpp"

template<class K, class V, Type::Integer I = size_t, PairType P = Pair>
struct InsertionMap:
	List<P<K, V>, I>,
	Collected<K, V, P>,
	SelfIdentified<InsertionMap<K, V, I, P>>
requires Type::Comparable::Threeway<K, K> {
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

#endif // CTL_CONTAINER_MAP_H
