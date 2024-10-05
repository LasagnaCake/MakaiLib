#ifndef CTL_CONTAINER_PAIR_H
#define CTL_CONTAINER_PAIR_H

#include "../templates.hpp"

CTL_NAMESPACE_BEGIN

template<typename TA, typename TB> struct Pair;

template<typename TA, typename TB>
struct Pairable {
	typedef TA AType;
	typedef TB BType;

	typedef Pair<AType, BType> PairType;
};

template<typename TA, typename TB>
struct Pair:
	Ordered,
	SelfIdentified<Pair<TA, TB>>,
	Pairable<TA, TB> {
	using SelfIdentified	= ::CTL::SelfIdentified<Pair<TA, TB>>;
	using Pairable			= ::CTL::Pairable<TA, TB>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Pairable::AType,
		typename Pairable::BType,
		typename Pairable::PairType
	;

	using typename Ordered::OrderType;

	AType a;
	BType b;

	constexpr Pair() = default;

	constexpr Pair(AType const& a):					a(a)					{}
	constexpr Pair(AType const& a, BType const& b):	a(a), b(b)				{}
	constexpr Pair(SelfType const& other):			a(other.a), b(other.b)	{}

	constexpr OrderType operator<=>(SelfType const& other) requires (
		Type::Comparable::Threeway<AType, AType>
	&&	Type::Comparable::Threeway<BType, BType>
	) {
		OrderType order = a <=> other.a;
		if (order == Order::EQUAL)
			return b <=> other.b;
		return order;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (
		Type::Comparable::Threeway<AType, AType>
	&&	!Type::Comparable::Threeway<BType, BType>
	) {
		return a <=> other.a;
	}

	template<Type::Constructible<AType, BType> TPair>
	constexpr TPair pair() const		{return TPair(a, b);	}

	template<Type::Constructible<AType, BType> TPair>
	constexpr operator TPair() const	{return pair<TPair>();	}
};

template<typename TKey, typename TValue>
struct KeyValuePair:
	Ordered,
	SelfIdentified<KeyValuePair<TKey, TValue>>,
	Pairable<TKey, TValue> {
	using SelfIdentified	= ::CTL::SelfIdentified<KeyValuePair<TKey, TValue>>;
	using Pairable			= ::CTL::Pairable<TKey, TValue>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Pairable::AType,
		typename Pairable::BType,
		typename Pairable::PairType
	;

	using typename Ordered::OrderType;

	AType	key;
	BType	value;

	constexpr KeyValuePair() = default;

	constexpr KeyValuePair(AType const& k):					key(k)									{}
	constexpr KeyValuePair(AType const& k, BType const& v):	key(k), value(v)						{}
	constexpr KeyValuePair(PairType const& other):			KeyValuePair(other.a, other.b)			{}
//	constexpr KeyValuePair(SelfType const& other):			KeyValuePair(other.key, other.value)	{}

	constexpr OrderType operator<=>(SelfType const& other) requires (
		Type::Comparable::Threeway<AType, AType>
	&&	Type::Comparable::Threeway<BType, BType>
	) {
		OrderType order = key <=> other.key;
		if (order == Order::EQUAL)
			return value <=> other.value;
		return order;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (
		Type::Comparable::Threeway<AType, AType>
	&&	!Type::Comparable::Threeway<BType, BType>
	) {
		return key <=> other.key;
	}

	constexpr PairType pair() const		{return PairType(key, value);	}
	constexpr operator PairType() const	{return pair();					}
};

template<typename TLeft, typename TRight>
struct LeftRightPair:
	Ordered,
	SelfIdentified<LeftRightPair<TLeft, TRight>>,
	Pairable<TLeft, TRight> {
	using SelfIdentified	= ::CTL::SelfIdentified<LeftRightPair<TLeft, TRight>>;
	using Pairable			= ::CTL::Pairable<TLeft, TRight>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Pairable::AType,
		typename Pairable::BType,
		typename Pairable::PairType
	;

	using typename Ordered::OrderType;

	AType	left;
	BType	right;

	constexpr OrderType operator<=>(SelfType const& other) requires (
		Type::Comparable::Threeway<AType, AType>
	&&	Type::Comparable::Threeway<BType, BType>
	) {
		OrderType order = left <=> other.left;
		if (order == Order::EQUAL)
			return right <=> other.right;
		return order;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (
		Type::Comparable::Threeway<AType, AType>
	&&	!Type::Comparable::Threeway<BType, BType>
	) {
		return left <=> other.left;
	}

	constexpr LeftRightPair() = default;

	constexpr LeftRightPair(AType const& l):					left(l)									{}
	constexpr LeftRightPair(AType const& l, BType const& r):	left(l), right(r)						{}
	constexpr LeftRightPair(PairType const& other):				LeftRightPair(other.a, other.b)			{}
//	constexpr LeftRightPair(SelfType const& other):				LeftRightPair(other.left, other.right)	{}

	constexpr PairType pair() const		{return PairType(left, right);	}
	constexpr operator PairType() const	{return pair();					}
};

template<typename T1, typename T2>
struct FirstSecondPair:
	Ordered,
	SelfIdentified<FirstSecondPair<T1, T2>>,
	Pairable<T1, T2> {
	using SelfIdentified	= ::CTL::SelfIdentified<FirstSecondPair<T1, T2>>;
	using Pairable			= ::CTL::Pairable<T1, T2>;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Pairable::AType,
		typename Pairable::BType,
		typename Pairable::PairType
	;

	using typename Ordered::OrderType;

	AType	first;
	BType	second;

	constexpr OrderType operator<=>(SelfType const& other) requires (
		Type::Comparable::Threeway<AType, AType>
	&&	Type::Comparable::Threeway<BType, BType>
	) {
		OrderType order = first <=> other.first;
		if (order == Order::EQUAL)
			return second <=> other.second;
		return order;
	}

	constexpr OrderType operator<=>(SelfType const& other)
	requires (
		Type::Comparable::Threeway<AType, AType>
	&&	!Type::Comparable::Threeway<BType, BType>
	) {
		return first <=> other.first;
	}

	constexpr FirstSecondPair() = default;

	constexpr FirstSecondPair(AType const& v1):						first(v1)									{}
	constexpr FirstSecondPair(AType const& v1, BType const& v2):	first(v1), second(v2)						{}
	constexpr FirstSecondPair(PairType const& other):				FirstSecondPair(other.a, other.b)			{}
//	constexpr FirstSecondPair(SelfType const& other):				FirstSecondPair(other.first, other.second)	{}

	constexpr PairType pair() const		{return PairType(first, second);	}
	constexpr operator PairType() const	{return pair();						}
};

namespace Type {
	template<class T>
	concept Pair = requires (T t) {
		typename T::AType;
		typename T::BType;
		//{t.template get<0>()} -> Type::Equal<typename T::AType>;
		//{t.template get<1>()} -> Type::Equal<typename T::BType>;
		requires Type::Convertible<T, ::CTL::Pair<typename T::AType, typename T::BType>>;
	};
}

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_PAIR_H
