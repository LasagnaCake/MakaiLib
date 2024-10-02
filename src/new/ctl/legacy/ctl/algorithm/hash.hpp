#ifndef CTL_ALGORITHM_HASH_H
#define CTL_ALGORITHM_HASH_H

#include "../ctypes.hpp"
#include "../namespace.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

struct Hasher {
	template <Type::Integer T>
	constexpr static usize hash(T const& value)		{return value;					}

	template <Type::Real T>
	constexpr static usize hash(T const& value)		{return *((uint32*)(&value));	}

	template <class T>
	constexpr static usize hash(T* const& value)	{return (usize)value;			}
};

template<class TValue, class THasher>
concept Hashable = requires (TValue v) {
	{THasher::hash(v)} -> Type::Equal<usize>;
};

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_HASH_H
