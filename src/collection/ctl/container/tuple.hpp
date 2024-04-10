#ifndef CTL_CONTAINER_TUPLE_H
#define CTL_CONTAINER_TUPLE_H

#include "../templates.hpp"

template<usize N, class T>
struct TupleItem: Typed<T> {
    T value;
};

template<usize N, class... Types>
struct TuplePack;

template<usize N>
struct TuplePack<N>{};



template<usize N, class T, class... Types>
struct TuplePack<N, T, Types...>:
	public TupleLeaf<N, T>,
	public TuplePack<N + 1, Types...>,
	Polyglot<T, Types...>
{
	template<usize INDEX>
	constexpr DataTypes::Types<INDEX>& get()
	requires (INDEX < DataTypes::COUNT) {
		return TupleItem<INDEX, DataTypes::Types<INDEX>>::value;
	}
};

template<usize N, class T, typename... Types>
T& get(TuplePack<N, T, Types...>& tup)
requires (N < TuplePack<N, T, Types...>::DataTypes::COUNT) {
    return tup.TupleItem<N, T>::value;
}

template<class... Types>
using Tuple = TuplePack<0, Types...>;

template<class... MemberTypes>
struct Reflective {
	typedef Tuple<MemberTypes&...> ReflectionType;
};

#endif // CTL_CONTAINER_TUPLE_H
