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
	using SelfIdentified	= SelfIdentified<Pair<TA, TB>>;
	using Pairable			= Pairable<TA, TB>;

	using
		SelfIdentified::SelfType
	;

	using
		Pairable::AType,
		Pairable::BType,
		Pairable::PairType
	;

	AType a;
	BType b;

	constexpr Pair()	{}

	constexpr Pair(AType const& a, BType const& b):	a(a), b(b)				{}
	constexpr Pair(SelfType const& other):			a(other.a), b(other.b)	{}

	constexpr operator<=>(SelfType const& other) requires (
		Type::Comparable::Threeway<AType, AType>
	&&	Type::Comparable::Threeway<BType, BType>
	) {
		ValueOrder order = a <=> other.a;
		if (order == ValueOrder::EQUAL)
			return b <=> other.b;
		return order;
	}

	template<Type::Constructible<AType, BType> TPair>
	constexpr TPair pair() const		{return TPair(a, b);	}

	template<Type::Constructible<AType, BType> TPair>
	constexpr operator TPair() const	{return pair<TPair>();	}
}

template<typename TKey, typename TValue>
struct KeyValuePair:
	Ordered,
	SelfIdentified<KeyValuePair<TKey, TValue>>,
	Pairable<TKey, TValue> {
	using SelfIdentified	= SelfIdentified<Pair<TKey, TValue>>;
	using Pairable			= Pairable<TKey, TValue>;

	using
		SelfIdentified::SelfType
	;

	using
		Pairable::AType,
		Pairable::BType,
		Pairable::PairType
	;

	AType	key;
	BType	value;

	constexpr KeyValuePair()	{}

	constexpr KeyValuePair(AType const& k, BType const& v):	key(k), value(v)						{}
	constexpr KeyValuePair(PairType const& other):			KeyValuePair(other.a, other.b)			{}
	constexpr KeyValuePair(SelfType const& other):			KeyValuePair(other.key, other.value)	{}

	constexpr PairType pair() const		{return PairType(key, value);	}
	constexpr operator PairType() const	{return pair();					}
};

template<typename TLeft, typename TRight>
struct LeftRightPair:
	Ordered,
	SelfIdentified<LeftRightPair<TLeft, TRight>>,
	Pairable<TLeft, TRight> {
	using SelfIdentified	= SelfIdentified<Pair<TLeft, TRight>>;
	using Pairable			= Pairable<TLeft, TRight>;

	using
		SelfIdentified::SelfType
	;

	using
		Pairable::AType,
		Pairable::BType,
		Pairable::PairType
	;

	AType	left;
	BType	right;

	constexpr LeftRightPair()	{}

	constexpr LeftRightPair(AType const& l, BType const& r):	left(l), right(r)						{}
	constexpr LeftRightPair(PairType const& other):				LeftRightPair(other.a, other.b)			{}
	constexpr LeftRightPair(SelfType const& other):				LeftRightPair(other.left, other.right)	{}

	constexpr PairType pair() const		{return PairType(left, right);	}
	constexpr operator PairType() const	{return pair();					}
};

template<typename T1, typename T2>
struct FirstSecondPair:
	Ordered,
	SelfIdentified<FirstSecondPair<T1, T2>>,
	Pairable<T1, T2> {
	using SelfIdentified	= SelfIdentified<Pair<T1, T2>>;
	using Pairable			= Pairable<T1, T2>;

	using
		SelfIdentified::SelfType
	;

	using
		Pairable::AType,
		Pairable::BType,
		Pairable::PairType
	;

	AType	first;
	BType	second;

	constexpr FirstSecondPair()	{}

	constexpr FirstSecondPair(AType const& v1, BType const& v2):	first(v1), second(v2)						{}
	constexpr FirstSecondPair(PairType const& other):				FirstSecondPair(other.a, other.b)			{}
	constexpr FirstSecondPair(SelfType const& other):				FirstSecondPair(other.first, other.second)	{}

	constexpr PairType pair() const		{return PairType(first, second);	}
	constexpr operator PairType() const	{return pair();						}
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_PAIR_H
