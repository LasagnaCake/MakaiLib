#ifndef CTL_ALGORITHM_BITWISE_H
#define CTL_ALGORITHM_BITWISE_H

#include "../ctypes.hpp"

CTL_NAMESPACE_BEGIN

constexpr usize highBit(usize n) {
	if (!n) return 0;
	usize r = 1;
	while (n >>= 1) r <<= 1;
	return r;
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_BITWISE_H
