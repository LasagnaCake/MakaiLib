#ifndef CTL_CONTAINER_PAIR_H
#define CTL_CONTAINER_PAIR_H

#include "../templates.hpp"
#include "tuple.hpp"

template<class K, class V>
struct Pair:
	Keyed<K>,
	Valued<V>,
	SelfIdentified<Pair<K, V>>,
	Ordered {
public:
	KeyType		key;
	ValueType	value;

	MAKE_REFLECTIVE(key, value);

	constexpr OrderType operator<=>(SelfType const& other)
	requires (Type::Comparable::Threeway<K, K> && Type::Comparable::Threeway<V, V>) {
		OrderType result = key <=> other.key;
		if (result != OrderType::EQUAL)
			return result;
		return value <=> other.value;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (Type::Comparable::Threeway<K, K> && !Type::Comparable::Threeway<V, V>) {
		return key <=> other.key;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (!Type::Comparable::Threeway<K, K> && Type::Comparable::Threeway<V, V>) {
		return value <=> other.value;
	}
};

template<class T>
concept PairType = requires {
	typename T::KeyType;
	typename T::ValueType;
} && Type::Derived<T, Pair<typename T::KeyType, typename T::ValueType>>;

template<class K, class V, PairType P>
struct Collected: Keyed<K>, Valued<V> {
	typedef P<K, V> PairType;
};

#endif // CTL_CONTAINER_PAIR_H
