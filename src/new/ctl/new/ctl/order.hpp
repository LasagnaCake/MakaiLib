#ifndef CTL_TYPE_ORDER_H
#define CTL_TYPE_ORDER_H

#include <compare>

#include "namespace.hpp"
#include "typeinfo.hpp"
#include "templates.hpp"

CTL_NAMESPACE_BEGIN

enum class StandardOrder: int {
	LESS = -1,
	EQUAL,
	GREATER,
	UNORDERED
};

namespace Base {
	struct Order {
		constexpr static auto LESS		= std::partial_ordering::less;
		constexpr static auto EQUAL		= std::partial_ordering::equivalent;
		constexpr static auto GREATER	= std::partial_ordering::greater;
		constexpr static auto UNORDERED	= std::partial_ordering::unordered;
	};
}

struct ValueOrder {
	struct Unspecified {};

	constexpr ValueOrder(StandardOrder const& value): val(value) {}

	constexpr ValueOrder(std::partial_ordering const& value) {
		if (value == Base::Order::LESS)			val = StandardOrder::LESS;
		else if (value == Base::Order::GREATER)	val = StandardOrder::GREATER;
		else if (value == Base::Order::EQUAL)	val = StandardOrder::EQUAL;
	}

	constexpr ValueOrder(std::weak_ordering const& value):
	ValueOrder(std::partial_ordering(value)) {}

	constexpr ValueOrder(std::strong_ordering const& value):
	ValueOrder(std::partial_ordering(value)) {}

	constexpr StandardOrder operator<=>(ValueOrder const& other) const {
		return  ValueOrder(val <=> other.val);
	}

	constexpr StandardOrder operator<=>(StandardOrder const& order) const {
		return  ValueOrder(val <=> order);
	}

	template<Type::Convertible<StandardOrder> T>
	constexpr StandardOrder operator<=>(T const& value)
	requires Type::Different<T, StandardOrder> {
		return  *this <=> (StandardOrder)value;
	}

	template<Type::Convertible<ValueOrder> T>
	constexpr StandardOrder operator<=>(T const& value)
	requires Type::Different<T, ValueOrder> {
		return  *this <=> ValueOrder(value).order();
	}

	constexpr bool operator==(StandardOrder const& order) const	{return val == order;		}
	constexpr bool operator==(ValueOrder const& other) const	{return val == other.val;	}
	
	[[nodiscard]]
	friend constexpr bool operator==(ValueOrder const& a, int)	{return a.val == StandardOrder::EQUAL;		}
	[[nodiscard]]
	friend constexpr bool operator==(int, ValueOrder const& b)	{return b == int();							}

	[[nodiscard]]
	friend constexpr bool operator<(ValueOrder const& a, int)	{return a.val == StandardOrder::LESS;		}
	[[nodiscard]]
	friend constexpr bool operator>(ValueOrder const& a, int)	{return a.val == StandardOrder::GREATER;	}
	[[nodiscard]]
	friend constexpr bool operator<=(ValueOrder const& a, int)	{return a.val == StandardOrder::LESS || a.val == StandardOrder::EQUAL;		}
	[[nodiscard]]
	friend constexpr bool operator>=(ValueOrder const& a, int)	{return a.val == StandardOrder::GREATER || a.val == StandardOrder::EQUAL;	}

	[[nodiscard]]
	friend constexpr bool operator<(int, ValueOrder const& b)	{return b < int();	}
	[[nodiscard]]
	friend constexpr bool operator>(int, ValueOrder const& b)	{return b > int();	}
	[[nodiscard]]
	friend constexpr bool operator<=(int, ValueOrder const& b)	{return b <= int();	}
	[[nodiscard]]
	friend constexpr bool operator>=(int, ValueOrder const& b)	{return b >= int();	}

	constexpr operator StandardOrder()	{return order();		}
	constexpr StandardOrder order()		{return val;			}

private:
	StandardOrder val = StandardOrder::UNORDERED;
};

template<typename T, typename T2>
constexpr ValueOrder operator<=>(T const& a, T const& b) {
	return a <=> b;
}

struct Ordered {
	typedef ValueOrder OrderType;
	using Order = StandardOrder;
};

CTL_NAMESPACE_END

#endif // CTL_TYPE_ORDER_H
