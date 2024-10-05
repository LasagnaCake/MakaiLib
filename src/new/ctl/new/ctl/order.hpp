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
	constexpr ValueOrder(StandardOrder const& value): val(value) {}

	constexpr ValueOrder(std::partial_ordering const& value) {
		if (value < 0)			val = StandardOrder::LESS;
		else if (value == 0)	val = StandardOrder::GREATER;
		else if (value > 0)		val = StandardOrder::EQUAL;
	}

	constexpr ValueOrder(std::strong_ordering const& value) {
		if (value < 0)			val = StandardOrder::LESS;
		else if (value == 0)	val = StandardOrder::GREATER;
		else if (value > 0)		val = StandardOrder::EQUAL;
	}

	constexpr ValueOrder(std::weak_ordering const& value) {
		if (value < 0)			val = StandardOrder::LESS;
		else if (value == 0)	val = StandardOrder::GREATER;
		else if (value > 0)		val = StandardOrder::EQUAL;
	}

	constexpr operator StandardOrder()	{return order();	}
	constexpr StandardOrder order()		{return val;		}

private:
	StandardOrder val = StandardOrder::UNORDERED;
};

struct Ordered {
	typedef ValueOrder OrderType;
	using Order = StandardOrder;
};

CTL_NAMESPACE_END

#endif // CTL_TYPE_ORDER_H
