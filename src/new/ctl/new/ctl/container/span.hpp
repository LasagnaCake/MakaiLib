#ifndef CTL_CONTAINER_SPAN_H
#define CTL_CONTAINER_SPAN_H

#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../namespace.hpp"
#include "iterator.hpp"

CTL_NAMESPACE_BEGIN

constexpr usize DYNAMIC = -1;

template<class TData, usize S = DYNAMIC, Type::Integer TIndex = usize>
struct Span:
	Iteratable<TData, TIndex> {

	constexpr Span() {}

	constexpr Span(PointerType const& data): data(data) {
	}

	constexpr Span(PointerType const& data, SizeType const& size): data(data), count(size) {
	}

	constexpr DataType& operator[](IndexType index) const {
		wrapBounds(index, count);
	}

private:
	DataType*	data	= nullptr;
	usize		count	= S;
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_SPAN_H
