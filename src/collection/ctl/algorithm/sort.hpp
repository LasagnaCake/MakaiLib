#ifndef CTL_ALGORITHM_SORTING_H
#define CTL_ALGORITHM_SORTING_H

#include "../container/iterator.hpp"
#include "memory.hpp"
#include "transform.hpp"

namespace Type {
	template <class T>
	concept Sortable = Comparable::Threeway<T, T> && Comparable::Equals<T, T>;
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
			usize j = i-1;
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
		if (sz < 2) return;
		if (sz == 2) {
			if (arr[0] > arr[1])
				swap(arr[0], arr[1]);
			return;
		}
		if (sz < 4) insertionSort(arr, sz);
		usize
			j		= 1,
			offset	= 0
		;
		auto
			prevOrder = arr[1] <=> arr[0],
			currentOrder = prevOrder
		;
		for (usize run = Math::max(highBit(sz) >> 4, 4); run < sz; run <<= 1) {
			for (usize i = 1; i < sz; ++i) {
				currentOrder = arr[i] <=> arr[i-1];
				if (currentOrder != prevOrder && currentOrder != ValueOrder::EQUAL) {
					if (j < run) {
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
			if (j == sz) {
				if (currentOrder == ValueOrder::LESS)
					reverse(arr, sz);
				return;
			}
			currentOrder = prevOrder = arr[1] <=> arr[0];
			offset = 0;
		}
	}
}

template <SortableIterator T>
constexpr void sort(T const& begin, T const& end) {
	Sorting::vivoSort(&*begin, end - begin);
}

#endif // CTL_ALGORITHM_FUNCTIONS_H
