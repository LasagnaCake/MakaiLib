#ifndef CTL_ALGORITHM_HASH_H
#define CTL_ALGORITHM_HASH_H

#include "../ctypes.hpp"

struct Hasher {
	template <Type::Integer T>
	constexpr static usize hash(T const& value)		{return value;					}

	template <Type::Float T>
	constexpr static usize hash(T const& value)		{return *((uint32*)(&value));	}

	template <class T>
	constexpr static usize hash(T* const& value)	{return (usize)value;			}
};

template<class TValue, class THasher>
concept Hashable = requires (TValue v) {
	{THasher::hash(v);} -> Type::Equal<usize>;
}

#endif // CTL_ALGORITHM_HASH_H
