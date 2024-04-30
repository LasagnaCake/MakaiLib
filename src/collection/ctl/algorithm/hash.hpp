#ifndef CTL_ALGORITHM_HASH_H
#define CTL_ALGORITHM_HASH_H

#include "../ctypes.hpp"

struct Hasher {
	template <Type::Integer I>
	constexpr static usize hash(I const& value) {return value;}

	template <Type::Float F>
	constexpr static usize hash(F const& value) {return *((uint32*)(&value));}
};

template<class T, class THasher>
concept Hashable = requires (T v) {
	{THasher::hash(v);} -> Type::Equal<usize>;
}

#endif // CTL_ALGORITHM_HASH_H
