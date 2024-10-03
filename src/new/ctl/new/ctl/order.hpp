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
	UNORDERED = int(1 << (sizeof(int) *8) - 1)
};

namespace Base {
	struct Order {
		constexpr static auto LESS		= std::partial_ordering::less;
		constexpr static auto EQUAL		= std::partial_ordering::equivalent;
		constexpr static auto GREATER	= std::partial_ordering::greater;
		constexpr static auto UNORDERED	= std::partial_ordering::unordered;
	};
}

struct ValueOrder: Base::Order {};

struct Ordered {
	typedef decltype(ValueOrder::LESS) OrderType;
	using Order = ValueOrder;
};

CTL_NAMESPACE_END

#endif // CTL_TYPE_ORDER_H
