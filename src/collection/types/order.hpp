#ifndef TYPE_ORDER_H
#define TYPE_ORDER_H

#include <compare>

enum class StandardOrder {
	LESS = -1,
	EQUAL,
	GREATER,
	UNORDERED
};

struct ValueOrder: public std::partial_ordering {
	constexpr static auto LESS		= std::partial_ordering::less;
	constexpr static auto EQUAL		= std::partial_ordering::equivalent;
	constexpr static auto GREATER	= std::partial_ordering::greater;
	constexpr static auto UNORDERED	= std::partial_ordering::unordered;
};


#endif // TYPE_ORDER_H
