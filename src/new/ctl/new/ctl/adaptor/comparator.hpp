#ifndef CTL_ADAPTOR_COMPARATOR_H
#define CTL_ADAPTOR_COMPARATOR_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "../templates.hpp"
#include "../order.hpp"

CTL_NAMESPACE_BEGIN

template <class A, class B>
struct Comparator;

template <class A, class B>
requires (Type::Comparable::Threeway<A, B>)
struct Comparator<A, B>: Ordered {
	using typename Ordered::OrderType;

	constexpr static OrderType compare(A const& a, B const& b) {
		return a <=> b;
	}

	constexpr static bool equals(A const& a, B const& b)
	requires Type::Comparable::Equals<A, B> {
		return a == b;
	}
};

namespace {
	template<class A, class B>
	concept NonStandardThreeway = (
		!Type::Comparable::Threeway<A, B>
	&&	(
			Type::Comparable::Lesser<A, B>
		||	Type::Comparable::Greater<A, B>
		)
	&&	Type::Comparable::Equals<A, B>
	);	
}

template <class A, class B>
requires (NonStandardThreeway<A, B>)
struct Comparator<A, B>: Ordered {
	using typename Ordered::OrderType;

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::Lesser<A, B> {
		if (a < b)	return Order::LESS;
		if (a == b)	return Order::EQUAL;
		return Order::GREATER;
	}

	constexpr static OrderType compare(A const& a, B const& b)
	requires Type::Comparable::Greater<A, B> {
		if (a > b)	return Order::GREATER;
		if (a == b)	return Order::EQUAL;
		return Order::LESS;
	}

	constexpr static bool equals(A const& a, B const& b)
	requires Type::Comparable::Equals<A, B> {
		return a == b;
	}
};

namespace Type::Comparator {
	template<class A, class B>
	concept Comparable = requires (A a, B b) {
		{::CTL::Comparator<A, B>::compare(a, b)} -> Type::Equal<ValueOrder>;
	};
}

CTL_NAMESPACE_END

#endif