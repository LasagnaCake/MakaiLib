#ifndef CTL_CONTAINER_PAIR_H
#define CTL_CONTAINER_PAIR_H

#include "../templates.hpp"
#include "../adapter/comparator.hpp"
#include "../meta/logic.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Container-specific type constraints.
namespace Type::Container {
	/// @brief Type must be a valid pair type.
	template<class T>
	concept Pair = requires (T t) {
		typename T::AType;
		typename T::BType;
		{t.getA()} -> EqualOrConst<typename T::AType&>;
		{t.getB()} -> EqualOrConst<typename T::BType&>;
		requires Constructible<T, typename T::AType, typename T::BType>;
	};
}

template<typename TA, typename TB> struct Pair;

/// @brief Custom comparator implementation for a pair-type.
template<Type::Container::Pair TPair>
struct PairComparator: Ordered {
	/// @brief Pair type.
	using PairType = TPair;

	/// @brief "A" type.
	using AType = typename TPair::AType;
	/// @brief "B" type.
	using BType = typename TPair::BType;

	using typename Ordered::OrderType;

	/// @brief Whether "A" type is comparable.
	constexpr static bool COMPARABLE_A = Type::Comparator::Comparable<AType, AType>;
	/// @brief Whether "B" type is comparable.
	constexpr static bool COMPARABLE_B = Type::Comparator::Comparable<BType, BType>;

	/// @brief Whether this comparator is valid.
	constexpr static bool IS_COMPARABLE = COMPARABLE_A || COMPARABLE_B;

	/// @brief Comparator for "A" type (if comparable).
	using AComparator	= Meta::DualType<COMPARABLE_A, Comparator<AType, AType>, void>;
	/// @brief Comparator for "B" type (if comparable).
	using BComparator	= Meta::DualType<COMPARABLE_B, Comparator<BType, BType>, void>;

	/// @brief Compares two pairs.
	/// @param a Left-hand side of the operation.
	/// @param b Right-hand side of the operation.
	/// @return Order between pairs.
	constexpr static OrderType compare(PairType const& a, PairType const& b)
	requires (COMPARABLE_A && COMPARABLE_B) {
		OrderType order = AComparator::compare(a.getA(), b.getA());
		if (order == Order::EQUAL)
			return BComparator::compare(a.getB(), b.getB());
		return order;
	}

	/// @brief Compares two pairs.
	/// @param a Left-hand side of the operation.
	/// @param b Right-hand side of the operation.
	/// @return Order between pairs.
	constexpr static OrderType compare(PairType const& a, PairType const& b)
	requires (COMPARABLE_A && !COMPARABLE_B) {
		return AComparator::compare(a.getA(), b.getA());
	}

	/// @brief Compares two pairs.
	/// @param a Left-hand side of the operation.
	/// @param b Right-hand side of the operation.
	/// @return Order between pairs.
	constexpr static OrderType compare(PairType const& a, PairType const& b)
	requires (!COMPARABLE_A && COMPARABLE_B) {
		return BComparator::compare(a.getB(), b.getB());
	}
};

/// @brief Tags the deriving class as a pair of values.
/// @tparam TA "A" type.
/// @tparam TB "B" type.
template<typename TA, typename TB>
struct Pairable {
	typedef TA AType;
	typedef TB BType;

	typedef Pair<AType, BType> PairType;
};

/// @brief A-B pair.
/// @tparam TA "A" type. 
/// @tparam TB "B" type.
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

	/// @brief "A" value.
	AType a;
	/// @brief "B" value.
	BType b;

	/// @brief Empty constructor.
	constexpr Pair() = default;
	/// @brief Constructs only `a`.
	/// @param a Value of `a`.
	constexpr Pair(AType const& a):					a(a)								{}
	/// @brief Constructs both `a` and `b`. 
	/// @param a Value of `a`.
	/// @param b Value of `b`.
	constexpr Pair(AType const& a, BType const& b):	a(a), b(b)							{}
	/// @brief Copy constructor.
	/// @param other `Pair` to copy from.
	constexpr Pair(SelfType const& other):			a(other.a), b(other.b)				{}
	template<Type::Container::Pair T>
	constexpr Pair(T const& other):					a(other.getA()), b(other.getB())	{}

	constexpr OrderType operator<=>(SelfType const& other) const
	requires (PairComparator<SelfType>::IS_COMPARABLE) {
		return PairComparator<SelfType>::compare(*this, other);
	}

	constexpr AType& getA()				{return a;	}
	constexpr BType& getB()				{return b;	}
	constexpr AType const& getA() const	{return a;	}
	constexpr BType const& getB() const	{return b;	}

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

	constexpr OrderType operator<=>(SelfType const& other) const
	requires (PairComparator<SelfType>::IS_COMPARABLE) {
		return PairComparator<SelfType>::compare(*this, other);
	}

	constexpr AType& getA()				{return key;	}
	constexpr BType& getB()				{return value;	}
	constexpr AType const& getA() const	{return key;	}
	constexpr BType const& getB() const	{return value;	}

	constexpr PairType pair() const		{return PairType(key, value);	}
//	constexpr operator PairType() const	{return pair();					}
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

	constexpr OrderType operator<=>(SelfType const& other) const
	requires (PairComparator<SelfType>::IS_COMPARABLE) {
		return PairComparator<SelfType>::compare(*this, other);
	}

	constexpr AType& getA()				{return left;	}
	constexpr BType& getB()				{return right;	}
	constexpr AType const& getA() const	{return left;	}
	constexpr BType const& getB() const	{return right;	}

	constexpr LeftRightPair() = default;

	constexpr LeftRightPair(AType const& l):					left(l)									{}
	constexpr LeftRightPair(AType const& l, BType const& r):	left(l), right(r)						{}
	constexpr LeftRightPair(PairType const& other):				LeftRightPair(other.a, other.b)			{}
//	constexpr LeftRightPair(SelfType const& other):				LeftRightPair(other.left, other.right)	{}

	constexpr PairType pair() const		{return PairType(left, right);	}
//	constexpr operator PairType() const	{return pair();					}
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

	constexpr OrderType operator<=>(SelfType const& other) const
	requires (PairComparator<SelfType>::IS_COMPARABLE) {
		return PairComparator<SelfType>::compare(*this, other);
	}

	constexpr AType& getA()				{return first;	}
	constexpr BType& getB()				{return second;	}
	constexpr AType const& getA() const	{return first;	}
	constexpr BType const& getB() const	{return second;	}

	constexpr FirstSecondPair() = default;

	constexpr FirstSecondPair(AType const& v1):						first(v1)									{}
	constexpr FirstSecondPair(AType const& v1, BType const& v2):	first(v1), second(v2)						{}
	constexpr FirstSecondPair(PairType const& other):				FirstSecondPair(other.a, other.b)			{}
//	constexpr FirstSecondPair(SelfType const& other):				FirstSecondPair(other.first, other.second)	{}

	constexpr PairType pair() const		{return PairType(first, second);	}
//	constexpr operator PairType() const	{return pair();						}
};

static_assert(Type::Comparator::Threeway<Pair<int, int>, Pair<int, int>>);
static_assert(Type::Comparator::Threeway<KeyValuePair<int, int>, KeyValuePair<int, int>>);
static_assert(Type::Comparator::Threeway<LeftRightPair<int, int>, LeftRightPair<int, int>>);
static_assert(Type::Comparator::Threeway<FirstSecondPair<int, int>, FirstSecondPair<int, int>>);

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_PAIR_H
