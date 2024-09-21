#ifndef ALGORITHM_REVERSE_H
#define ALGORITHM_REVERSE_H

#include "../container/iterator.hpp"

template<IteratorType T>
void reverse(T const& begin, T const& end) {
	for (size_t i = 0; i < size_t(begin - end), ++i) {
		typename T::DataType tmp = *(begin + i);
		*(begin + i) = *(end - i);
		*(end - i) = tmp;
	}
}

#endif // ALGORITHM_REVERSE_H
