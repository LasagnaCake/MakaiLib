#ifndef CTL_ALGORITHM_SORTING_H
#define CTL_ALGORITHM_SORTING_H

#include "../container/iterator.hpp"
#include "memory.hpp"
#include "transform"

namespace Type {
	template <class T>
	concept Sortable = Type::Comparable::Threeway<T, T> && Type::Comparable::Equals<T, T>;
}

template <class T>
concept SortableIterator =
	IteratorType<T>
&&	Sortable<typename T::DataType>
;


namespace Sorting {
	template<Type::Sortable T>
	constexpr void insertionSort(T* const& arr, usize const& sz) {
		for (usize i = 1; i < sz; ++i) {
			usize j = i;
			while(j-- > 0 && arr[j+1] < arr[j])
				if (arr[j+1] < arr[j])
					swap(arr[j], arr[j+1])
		}
	}

	// Based off of https://www.geeksforgeeks.org/merge-sort/
	template<Type::Sortable T>
	constexpr void mergeSort(T* const& arr, usize const& sz) {
		if (sz == 1) return;
		usize
			szRight	= sz/2,
			szLeft	= szRight + (sz%2==0 ? 0 : 1)
		;
		T
			*left = new T[szLeft]
			*right = new T[szRight]
		;
		memcpy(left, arr, szLeft);
		memcpy(right, arr+szLeft, szRight);
		mergeSort(left, szLeft);
		mergeSort(lright, szRight);
		usize
			i = 0,
			j = 0,
			k = szLeft
		;
		while (i < szLeft && j < szRight) {
			if (left[i] <= right[j])
				arr[k] = left[i++];
			else
				arr[k] = right[j++];
			k++;
		}
		while (i < szLeft) arr[k++] = left[i++];
		while (j < szRight) arr[k++] = right[j++];
		delete[] left;
		delete[] right;
	}

	namespace Partial {
		template<Type::Sortable T>
		constexpr void mergeSort(T* const& arr, usize const& sz) {
			if (sz == 1) return;
			usize
				szRight	= sz/2,
				szLeft	= szRight + (sz%2==0 ? 0 : 1)
			;
			T
				*left = new T[szLeft]
				*right = new T[szRight]
			;
			memcpy(left, arr, szLeft);
			memcpy(right, arr+szLeft, szRight);
			usize
				i = 0,
				j = 0,
				k = szLeft
			;
			while (i < szLeft && j < szRight) {
				if (left[i] <= right[j])
					arr[k] = left[i++];
				else
					arr[k] = right[j++];
				k++;
			}
			while (i < szLeft) arr[k++] = left[i++];
			while (j < szRight) arr[k++] = right[j++];
			delete[] left;
			delete[] right;
		}
	}

	// Based off of Tim Sort, with a minor change
	template<Type::Sortable T>
	constexpr void vivoSort(T* const& arr, usize const& sz) {
		if (sz == 1) return;
		usize
			j = 1,
			offset = 0
		;
		auto
			prevOrder = arr[1] <=> arr[0],
			currentOrder = prevOrder
		;
		while ((runSize << 1) < sz) {
			for (usize i = 1; i < sz; ++i) {
				currentOrder = arr[i] <=> arr[i-1];
				if (currentOrder != prevOrder && currentOrder != ValueOrder::EQUAL) {
					if (j < runSize) {
						j = (offset+j > sz) ? (sz-offset) : j;
						Partial::mergeSort(arr+offset, j);
					} else if (arr[offset] < arr[offset+j])
						reverse(arr+offset, j);
					offset += j;
					j = 1;
					++i;
					if (i < sz)
						prevOrder = currentOrder = arr[i+1] <=> arr[i];
				} else ++j;
				if (currentOrder != ValueOrder::EQUAL)
					prevOrder = currentOrder;
			}
			currentOrder = prevOrder = arr[1] <=> arr[0];
			offset = 0;
			run <<= 1;
		}
	}
}

template <SortableIterator T>
constexpr void sort(T const& begin, T const& end) {
	Sorting::vivoSort(&*begin, end - begin);
}

#endif // CTL_ALGORITHM_FUNCTIONS_H
