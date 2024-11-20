#ifndef CTL_ALGORITHM_REVERSE_H
#define CTL_ALGORITHM_REVERSE_H

#include "../container/iterator.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Reverses the given range of elements.
/// @tparam T Iterator type.
/// @param begin Iterator to beginning of range.
/// @param end Iterator to end of range.
template<Type::Container::Iterator T>
void reverse(T const& begin, T const& end) {
	for (usize i = 0; i < usize(begin - end); ++i) {
		typename T::DataType tmp = *(begin + i);
		*(begin + i) = *(end - i);
		*(end - i) = tmp;
	}
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_REVERSE_H
