#ifndef CTL_ADAPTOR_COMPARATOR_H
#define CTL_ADAPTOR_COMPARATOR_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "../templates.hpp"
#include "../order.hpp"

CTL_NAMESPACE_BEGIN

template <class A, class B>
struct Comparator;

namespace Type::Comparable::NonStandard {
	template<class A, class B>
	concept FullHouseThreeway =
		!Type::Comparable::Threeway<A, B>
	&&	Type::Comparable::Greater<A, B>
	&&	Type::Comparable::Equals<A, B>
	&&	Type::Comparable::Lesser<A, B>
	;

	template<class A, class B>
	concept GreaterLesserThreeway =
		!Type::Comparable::Threeway<A, B>
	&&	Type::Comparable::Greater<A, B>
	&&	!Type::Comparable::Equals<A, B>
	&&	Type::Comparable::Lesser<A, B>
	;

	template<class A, class B>
	concept GreaterEqualsThreeway =
		!Type::Comparable::Threeway<A, B>
	&&	Type::Comparable::Greater<A, B>
	&&	Type::Comparable::Equals<A, B>
	&&	!Type::Comparable::Lesser<A, B>
	;

	template<class A, class B>
	concept LesserEqualsThreeway =
		!Type::Comparable::Threeway<A, B>
	&&	!Type::Comparable::Greater<A, B>
	&&	Type::Comparable::Equals<A, B>
	&&	Type::Comparable::Lesser<A, B>
	;

	template<class A, class B>
	concept Threeway =
		FullHouseThreeway<A, B>
	||	GreaterLesserThreeway<A, B>
	||	LesserEqualsThreeway<A, B>
	||	GreaterEqualsThreeway<A, B>
	;
}

namespace Type::Comparable {
	template<class A, class B>
	concept AnyThreeway =
		Type::Comparable::Threeway<A, B>
	||	Type::Comparable::NonStandard::Threeway<A, B>
	;
}

template <class A, class B>
requires Type::Comparable::AnyThreeway<A, B>
struct Comparator<A, B>: Ordered {
	using typename Ordered::OrderType;

	constexpr static OrderType compare(A const& a, B const& b)
	requires (Type::Comparable::Threeway<A, B>) {
		return OrderType(a <=> b);
	}

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::NonStandard::FullHouseThreeway<A, B> {
		if (a < b)	return Order::LESS;
		if (a == b)	return Order::EQUAL;
		if (a > b)	return Order::GREATER;
		return Order::UNORDERED;
	}

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::NonStandard::GreaterLesserThreeway<A, B> {
		if (a < b)	return Order::LESS;
		if (a > b)	return Order::GREATER;
		return Order::EQUAL;
	}

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::NonStandard::LesserEqualsThreeway<A, B> {
		if (a < b)	return Order::LESS;
		if (a == b)	return Order::EQUAL;
		return Order::GREATER;
	}

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::NonStandard::GreaterEqualsThreeway<A, B> {
		if (a > b)	return Order::GREATER;
		if (a == b)	return Order::EQUAL;
		return Order::LESS;
	}

	constexpr static bool equals(A const& a, B const& b)
	requires Type::Comparable::Equals<A, B> {
		return a == b;
	}

	constexpr static bool equals(A const& a, B const& b)
	requires (!Type::Comparable::Equals<A, B> && Type::Comparable::Threeway<A, B>) {
		return compare(a, b) == Order::EQUAL;
	}

	constexpr static bool greater(A const& a, B const& b) {
		return compare(a, b) == StandardOrder::GREATER;
	}

	constexpr static bool lesser(A const& a, B const& b) {
		return compare(a, b) == StandardOrder::LESS;
	}

	constexpr static bool greaterEquals(A const& a, B const& b) {
		return compare(a, b) == StandardOrder::GREATER || equals(a, b);
	}

	constexpr static bool lesserEquals(A const& a, B const& b) {
		return compare(a, b) == StandardOrder::LESS || equals(a, b);
	}
};

namespace Type::Comparator {
	template<class A, class B>
	concept Threeway = requires (A a, B b) {
		{::CTL::Comparator<A, B>::compare(a, b)} -> Type::Equal<ValueOrder>;
	};

	template<class A, class B>
	concept Equals = requires (A a, B b) {
		{::CTL::Comparator<A, B>::equals(a, b)} -> Type::Equal<bool>;
	};

	template<class A, class B>
	concept Comparable = Threeway<A, B> && Equals<A, B>;
}

template<class T>
using SimpleComparator = Comparator<T, T>;

CTL_NAMESPACE_END

#endif