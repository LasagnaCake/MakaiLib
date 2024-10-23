#ifndef CTL_ALGORITHM_BITWISE_H
#define CTL_ALGORITHM_BITWISE_H

#include "../ctypes.hpp"
#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

constexpr usize bitWidth(usize n) {
	if (!n) return 0;
	usize r = 1;
	while (n >>= 1) ++r;
	return r;
}

constexpr usize highBit(usize n) {
	return 1 << (bitWidth(n)-1);
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_BITWISE_H
