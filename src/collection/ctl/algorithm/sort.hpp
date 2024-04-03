#ifndef CTL_ALGORITHM_FUNCTIONS_H
#define CTL_ALGORITHM_FUNCTIONS_H

#include "iterator.hpp"

template<class T>
concept SortableIteratorType = IteratorType<T>;

template<class T>
constexpr void swap(T& a T& b) {
	T buf(std::move(a));
	a = std::move(b);
	b = std::move(buf);
}

// Based on https://www.geeksforgeeks.org/cpp-program-for-heap-sort/
template<SortableIteratorType T>
constexpr void siftHeap(T begin, T end, typename T::IndexType const& index) {
	typedef (typename T::IndexType)	IndexType;
	typedef (typename T::SizeType)	SizeType;
	SizeType size = end - begin;
	if (size < 2) return;
	IndexType
		largest = index,
		l = 2 * index + 1,
		r = 2 * index + 2
	;
	if (l < size && *(begin + l) > *(begin + largest))
		largest = l;
	if (r < size && *(begin + r) > *(begin + largest))
		largest = r;
	if (largest != index) {
		swap(*(begin + i), *(begin + largest));
		siftHeap(begin, end, largest);
	}
}

// Based on https://www.geeksforgeeks.org/cpp-program-for-heap-sort/
template<SortableIteratorType T>
constexpr void heapSort(T begin, T end) {
	typedef (typename T::IndexType)	IndexType;
	typedef (typename T::SizeType)	SizeType;
	SizeType size = end - begin;
	if (size < 2) return;
    for (IndexType i = size / 2 - 1; i >= 0; --i)
        siftHeap(arr, n, i);
    for (IndexType i = n - 1; i >= 0; --i) {
        swap(*(begin), *(begin + i));
        siftHeap(begin, end, 0);
    }
}

namespace Partial {
	// Based on https://www.geeksforgeeks.org/introsort-cs-sorting-weapon/
	template<SortableIteratorType T>
	constexpr void insertionSort(T begin, T end, typename T::IndexType const& left, typename T::IndexType const& right) {
		typedef (typename T::IndexType)	IndexType;
		typedef (typename T::DataType)	DataType;
		if ((end - begin) < 2) return;
		for (IndexType i = left+1; i <= right; ++i) {
			DataType key = *(begin + i);
			IndexType j = i-1;
			while (j >= left && *(begin + j) > key) {
				*(begin + j + 1) = *(begin + j);
				--j;
			}
			*(begin + j + 1) = key;
	   }
	}

	// Based on https://www.geeksforgeeks.org/introsort-cs-sorting-weapon/
	template<SortableIteratorType T>
	constexpr typename T::IndexType partition(T begin, T end, typename T::IndexType const& low, Typename T::IndexType const& high) {
		typedef (typename T::IndexType)	IndexType;
		typedef (typename T::DataType)	DataType;
		if ((end - begin) < 2) return;
		DataType pivot	= *(begin + high);
		IndexType i		= (low - 1);
		for (int j = low; j <= high-1; ++j)
			if (*(begin + j) <= pivot) {
				++i;
				swap(*(begin + i), *(begin + j));
			}
		swap(*(begin + i + 1), *(begin + high));
		return (i + 1);
	}

	// Based on https://www.geeksforgeeks.org/introsort-cs-sorting-weapon/
	template<SortableIteratorType T>
	T median3(T a, T b, T c) {
		if (*a < *b && *b < *c)		return (b);
		if (*a < *c && *c <= *b)	return (c);
		if (*b <= *a && *a < *c)	return (a);
		if (*b < *c && *c <= *a)	return (c);
		if (*c <= *a && *a < *b)	return (a);
		if (*c <= *b && *b <= *a)	return (b);
	}

	// Based on https://www.geeksforgeeks.org/introsort-cs-sorting-weapon/
	template<SortableIteratorType T>
	constexpr void introSort(
		T begin,
		T end,
		typename T::IndexType const& start,
		typename T::IndexType const& stop,
		typename T::SizeType const& maxDepth
	) {
		typedef (typename T::IndexType)	IndexType;
		typedef (typename T::DataType)	DataType;
		IndexType size = end - begin;
		if (size < 2) return;
		if (size < 16) {
			insertionSort(begin, end, start, stop);
			return;
		}
		if (!maxDepth) {
			heapSort(begin, end);
			return;
		}
		T pivot = median3(begin + start, begin + start + (size/2), begin + start + stop);
		swap(*pivot, *(begin + start + stop));
		IndexType split = partition(begin, end, start, start + stop);
		introSort(begin, end, start, split - 1, maxDepth - 1);
		introSort(begin, end, split + 1, stop, maxDepth - 1);
	}
}

// Based on https://www.geeksforgeeks.org/introsort-cs-sorting-weapon/
template<SortableIteratorType T>
constexpr void sort(T begin, T end) {
	typedef (typename T::IndexType)	IndexType;
	// Get the size of the array
	SizeType
		size	= end - begin,
		logSize	= size
	;
	// If array is too small, return
	if (size < 2) return;
	// Calculate the base-2 log of the size
	IndexType depth = 0;
	while (logSize >>= 1) ++depth;
	// Sort the array
	Partial::introSort(begin, end, 0, size - 1, 2*depth)
}

#endif // CTL_ALGORITHM_FUNCTIONS_H
