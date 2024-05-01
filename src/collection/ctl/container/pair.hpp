#ifndef CTL_CONTAINER_PAIR_H
#define CTL_CONTAINER_PAIR_H

#include "../templates.hpp"
#include "tuple.hpp"

template<class TKey, class TValue>
struct Pair:
	Keyed<TKey>,
	Valued<TValue>,
	SelfIdentified<Pair<TKey, TValue>>,
	Ordered {
public:
	KeyType		key;
	ValueType	value;

	constexpr OrderType operator<=>(SelfType const& other)
	requires (Type::Comparable::Threeway<KeyType, KeyType> && Type::Comparable::Threeway<ValueType, ValueType>) {
		OrderType result = key <=> other.key;
		if (result != OrderType::EQUAL)
			return result;
		return value <=> other.value;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (Type::Comparable::Threeway<KeyType, KeyType> && !Type::Comparable::Threeway<ValueType, ValueType>) {
		return key <=> other.key;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (!Type::Comparable::Threeway<KeyType, KeyType> && Type::Comparable::Threeway<ValueType, ValueType>) {
		return value <=> other.value;
	}

	constexpr OrderType operator==(SelfType const& other)
	requires (Type::Comparable::Equals<KeyType, KeyType> && Type::Comparable::Equals<ValueType, ValueType>) {
		if (key == other.key)
			return value == other.value;
		return false;
	}

	constexpr OrderType operator==(SelfType const& other)
	requires (Type::Comparable::Equals<KeyType, KeyType> && !Type::Comparable::Equals<ValueType, ValueType>) {
		return key == other.key;
	}

	constexpr OrderType operator==(SelfType const& other)
	requires (!Type::Comparable::Equals<KeyType, KeyType> && Type::Comparable::Equals<ValueType, ValueType>) {
		return value == other.value;
	}
};

template<class T>
concept PairType = requires {
	typename T::KeyType;
	typename T::ValueType;
} && Type::Derived<T, Pair<typename T::KeyType, typename T::ValueType>>;

template<class TKey, class TValue, PairType TPair<K, V>>
struct Collected: Keyed<TKey>, Valued<TValue> {
	typedef TPair PairType;
};

#endif // CTL_CONTAINER_PAIR_H
