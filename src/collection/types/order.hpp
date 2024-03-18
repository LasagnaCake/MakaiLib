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
	constexpr static LESS		= std::partial_ordering::less;
	constexpr static EQUAL		= std::partial_ordering::equivalent;
	constexpr static GREATER	= std::partial_ordering::greater;
	constexpr static UNORDERED	= std::partial_ordering::unordered;
};


#endif // TYPE_ORDER_H
