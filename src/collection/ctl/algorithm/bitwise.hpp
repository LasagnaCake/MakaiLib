#ifndef CTL_ALGORITHM_BITWISE_H
#define CTL_ALGORITHM_BITWISE_H

#include "../ctypes.hpp"

constexpr usize highBit(usize n) {
	if (!n) return 0;
	usize r = 1;
	while (n >>= 1) r <<= 1;
	return r;
}

#endif // CTL_ALGORITHM_BITWISE_H
