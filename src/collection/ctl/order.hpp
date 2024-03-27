#ifndef CTL_TYPE_ORDER_H
#define CTL_TYPE_ORDER_H

#include <compare>

#include "typeinfo.hpp"

enum class StandardOrder: int {
	LESS = -1,
	EQUAL,
	GREATER,
	UNORDERED = int(1 << (sizeof(int) *8) - 1)
};

struct ValueOrder: public std::partial_ordering {
	constexpr static auto LESS		= std::partial_ordering::less;
	constexpr static auto EQUAL		= std::partial_ordering::equivalent;
	constexpr static auto GREATER	= std::partial_ordering::greater;
	constexpr static auto UNORDERED	= std::partial_ordering::unordered;
};


#endif // CTL_TYPE_ORDER_H
